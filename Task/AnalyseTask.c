/*
 * AnalyseTask.c
 *
 *  Created on: 2021年1月8日
 *      Author: loyer
 */
#include "task.h"

void analyse_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	char *res = NULL;
	char net = 0;
	bool needDecrypt = false;
	char *DecryptionBuf = NULL;
	char *DecryptionTemBuf = NULL;
	OS_FLAGS flag;
	struct STRUCT_USART_Fram *fram = NULL;
#if PRINT_STK_USED
	CPU_STK_SIZE n_free;
	CPU_STK_SIZE n_used;
#endif
	while (1)
	{
		//等待数据接收
		flag = OSFlagPend(
				(OS_FLAG_GRP*) &EventFlags,  //事件标志组
				(OS_FLAGS) FLAG_USART1_RxED + FLAG_USART3_RxED
						+ FLAG_USART2_RxED, //事件位
				(OS_TICK) 0,    //超时时间
				(OS_OPT) OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME, //等待置位并清除
				(CPU_TS*) 0,    //时间戳
				(OS_ERR*) &err); //错误码

		//判断进入的标志位
		if (flag == FLAG_USART1_RxED)
		{
			fram = &USART1_Fram;
			net = '3';
			needDecrypt = false;
		}
		else if (flag == FLAG_USART3_RxED)
		{
			fram = &F4G_Fram;
			net = '1';
			needDecrypt = true;
		}
		else if (flag == FLAG_USART2_RxED)
		{
			fram = &WIFI_Fram;
			net = '2';
			needDecrypt = true;
		}
		res = strchr((const char *) fram->RxBuf, '[');
		if (res && strchr((const char *) fram->RxBuf, ']'))
		{
			res += 1; //跳过字符'['
			if (strchr(res, '[')) //判断是否还有字符'[',防止数据出错
			{
				res = strchr(res, '[');
				res += 1; //跳过字符'['
			}
			if (strlen(res) > 8)  //跳过空心跳
			{
				DecryptionTemBuf = mymalloc(BASE64_BUF_LEN);
				DecryptionBuf = mymalloc(BASE64_BUF_LEN);
				memset(DecryptionTemBuf, '\0', BASE64_BUF_LEN);
				memset(DecryptionBuf, '\0', BASE64_BUF_LEN);
				res = strtok(res, "]"); //取出实际的数据
				if (needDecrypt)
				{
					needDecrypt = false;
					base64_decode(res, (unsigned char *) DecryptionTemBuf);
				}
				else
				{
					strcpy(DecryptionTemBuf, res);
				}
				DEBUG("%c>>%s\r\n", net, DecryptionTemBuf);
				res = strchr(DecryptionTemBuf, ','); //跳过设备编号
				res += 1;  //跳过字符','
				DecryptionBuf[0] = net;
				DecryptionBuf[1] = ','; //添加标识数据头
				strcat(DecryptionBuf, res);
				myfree(DecryptionTemBuf);  //释放临时资源
				//推送消息到数据处理任务
				OSTaskQPost((OS_TCB*) &Process_SeverTaskTCB, //待发送消息的任务控制块
						(void*) DecryptionBuf,      //待发送的数据
						(OS_MSG_SIZE) strlen(DecryptionBuf),      //发送的数据大小(字节数)
						(OS_OPT) OS_OPT_POST_FIFO,  //添加到消息队列尾
						(OS_ERR*) &err);     //返回的错误码
			}
		}
		else
		{
			if (flag == FLAG_USART3_RxED)
			{
				OSFlagPost((OS_FLAG_GRP*) &EventFlags, //对应的事件标志组
						(OS_FLAGS) FLAG_F4G_AT, //事件位
						(OS_OPT) OS_OPT_POST_FLAG_SET, //选择置位
						(OS_ERR*) &err); //错误码
			}
			else if (flag == FLAG_USART2_RxED)
			{
				OSFlagPost((OS_FLAG_GRP*) &EventFlags, //对应的事件标志组
						(OS_FLAGS) FLAG_WIFI_AT, //事件位
						(OS_OPT) OS_OPT_POST_FLAG_SET, //选择置位
						(OS_ERR*) &err); //错误码
			}
		}
#if PRINT_STK_USED
		OSTaskStkChk((OS_TCB *) 0, &n_free, &n_used, &err);
		DEBUG("AnalyseTask::free=%u,used=%u\r\n", n_free, n_used);
#endif
		OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_PERIODIC, &err);
	}
}

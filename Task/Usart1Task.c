/*
 * Usart1Task.c
 *
 *  Created on: 2021年1月7日
 *      Author: HHS007
 */
#include "task.h"

void usart1_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	char *res = NULL;
	u16 DecryptionLen = 0;
	char *DecryptionBuf = NULL;
	while (1)
	{
		//等待数据接收
		OSFlagPend((OS_FLAG_GRP*) &EventFlags,  //事件标志组
				(OS_FLAGS) 0x10, //事件位
				(OS_TICK) 0,    //超时时间
				(OS_OPT) OS_OPT_PEND_FLAG_SET_ALL + OS_OPT_PEND_FLAG_CONSUME, //等待置位并清除
				(CPU_TS*) 0,    //时间戳
				(OS_ERR*) &err); //错误码

		res = strchr((const char *) USART1_Fram.RxBuf, '[');
		if (res && strchr((const char *) USART1_Fram.RxBuf, ']'))
		{
			res += 1; //跳过字符'['
			if (strchr(res, '[')) //判断是否还有字符'[',防止数据出错
			{
				res = strchr(res, '[');
			}
			DecryptionBuf = mymalloc(strlen(res));
			res = strchr(res, ','); //跳过设备编号
			res += 1;  //跳过字符','
			strcpy(DecryptionBuf, "3,");  //添加串口1标识数据头
			strcat(DecryptionBuf, strtok(res, "]"));
			memset(USART1_Fram.RxBuf, '\0', USART1_Fram.AccessLen); //清空接收缓冲区
			DecryptionLen = strlen(DecryptionBuf);
			if (DecryptionLen > 3)
			{
				//推送消息到数据处理任务
				OSTaskQPost((OS_TCB*) &Process_SeverTaskTCB, //待发送消息的任务控制块
						(void*) DecryptionBuf,      //待发送的数据
						(OS_MSG_SIZE) DecryptionLen,       //发送的数据大小(字节数)
						(OS_OPT) OS_OPT_POST_FIFO,  //添加到消息队列尾
						(OS_ERR*) &err);     //返回的错误码
			}
			else
			{
				myfree(DecryptionBuf);  //如果数据量过少,则直接释放资源
			}
		}
		OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_PERIODIC, &err);
	}
}

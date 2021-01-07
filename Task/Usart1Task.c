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
	while (1)
	{
		//等待数据接收
		OSFlagPend((OS_FLAG_GRP*) &EventFlags,  //事件标志组
				(OS_FLAGS) 0x10, //事件位
				(OS_TICK) 0,    //超时时间
				(OS_OPT) OS_OPT_PEND_FLAG_SET_ALL + OS_OPT_PEND_FLAG_CONSUME, //等待置位并清除
				(CPU_TS*) 0,    //时间戳
				(OS_ERR*) &err); //错误码

		if (strchr((const char *)USART1_Fram.RxBuf, '[') && strchr((const char *)USART1_Fram.RxBuf, ']'))
		{
			int DecryptionLen = 0; //排除空心跳
			char *res = (char *) USART1_Fram.RxBuf;
			u16 deDataCnt = 0;
			while (*res != '[')
			{
				res++;
			}
			while (*res == '[')
				res++;

			while (*res != ']')
			{
				DecryptionLen++;
				USART1_Fram.DeData[deDataCnt++] = *res;
				res++;
			}
			memset(USART1_Fram.RxBuf, '\0', USART1_Fram.AccessLen);
			if (DecryptionLen > 1)
			{
				mySplit(&USART1_Fram, ",");
			}
		}
		OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_PERIODIC, &err);
	}
}

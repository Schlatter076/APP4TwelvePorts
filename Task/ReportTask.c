/*
 * TcpTask.c
 *
 *  Created on: 2021年1月7日
 *      Author: HHS007
 */
#include "task.h"

void report_task(void *p_arg)
{
	u8 *mes;
	OS_MSG_SIZE size;
	OS_ERR err;
	while (1)
	{
		//请求消息
		mes = OSTaskQPend((OS_TICK) 0,    //超时
				(OS_OPT) OS_OPT_PEND_BLOCKING,  //未收到数据时阻塞
				(OS_MSG_SIZE*) &size,     //指向接收到的数据大小
				(CPU_TS*) 0,       //时间戳
				(OS_ERR*) &err);  //错误码
		//成功读取到消息,开始上报

		OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TIME_PERIODIC, &err); //延时1s,将调度权交给CPU
	}
}

/*
 * ProcessServerTask.c
 *
 *  Created on: 2021年1月7日
 *      Author: loyer
 */
#include "task.h"

void process_server_task(void *p_arg)
{
	char *mes;
	char *tem;
	u8 net = 0;
	u8 cmd = 0;
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

		tem = strtok(mes, ",");
		net = atoi(tem);
		tem = strtok(NULL, ",");
		cmd = atoi(tem);
		tem = strtok(NULL, ",");
		ProcessServerCmd(net, cmd, tem);
		myfree(mes);  //释放资源
		OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_PERIODIC, &err);
	}
}

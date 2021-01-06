/*
 * task.c
 *
 *  Created on: 2021年1月5日
 *      Author: HHS007
 */
#include "task.h"

u8 *START_TASK_STK;
//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
	OSStatTaskCPUUsageInit(&err);  	//统计任务
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
	CPU_IntDisMeasMaxCurReset();
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	//使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);
#endif

	OS_CRITICAL_ENTER();	//进入临界区

	OS_CRITICAL_EXIT();	//退出临界区
	OSTaskDel((OS_TCB*) 0, &err);	//删除start_task任务自身
	myfree(START_TASK_STK);
}


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
//	CPU_INT32U cpu_clk_freq;
//	CPU_INT32U cnts;
#if PRINT_STK_USED
	CPU_STK_SIZE n_free;
	CPU_STK_SIZE n_used;
#endif
	CPU_SR_ALLOC();
	p_arg = p_arg;
	CPU_Init();
//	cpu_clk_freq = BSP_CPU_ClkFreq(); /* Determine SysTick reference freq.                    */
//	cnts = cpu_clk_freq / (CPU_INT32U) OSCfg_TickRate_Hz; /* Determine nbr SysTick increments                     */
//	OS_CPU_SysTickInit(cnts); /* Init uC/OS periodic time src (SysTick).              */

#if OS_CFG_STAT_TASK_EN > 0u
	OSStatTaskCPUUsageInit(&err);  	//统计任务
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
	CPU_IntDisMeasMaxCurReset();
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	//使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED, 1, &err);
#endif

	OS_CRITICAL_ENTER()
	;	//进入临界区
	OSFlagCreate((OS_FLAG_GRP*) &EventFlags, //指向事件标志组
			(CPU_CHAR*) "Event Flags",	//名字
			(OS_FLAGS) 0,				//事件标志组初始值
			(OS_ERR*) &err);			//错误码
	OSTmrCreate((OS_TMR *) &Stmr3,		 //定时器
			(CPU_CHAR *) "Stmr3",	//定时器名字
			(OS_TICK) 300,			//300*10=3S
			(OS_TICK) 0,   			//无周期
			(OS_OPT) OS_OPT_TMR_ONE_SHOT, 	//单次定时器
			(OS_TMR_CALLBACK_PTR) Stmr3_callback,	//定时器2回调函数
			(void *) 0,			//参数为0
			(OS_ERR *) &err);		//返回的错误码
	OSTmrCreate((OS_TMR *) &Stmr4,		//定时器
			(CPU_CHAR *) "Stmr4",		//定时器名字
			(OS_TICK) 0,			//0
			(OS_TICK) 10,          //10*10=100ms
			(OS_OPT) OS_OPT_TMR_PERIODIC, //周期模式
			(OS_TMR_CALLBACK_PTR) Stmr4_callback, //定时器回调函数
			(void *) 0,			//参数为0
			(OS_ERR *) &err);	//返回的错误码
	OSTmrStart(&Stmr4, &err);	//开启定时器4
	OSTaskCreate((OS_TCB *) &BatTaskTCB,		//任务控制块
			(CPU_CHAR *) "bat task", 		//任务名字
			(OS_TASK_PTR) bat_task, 			//任务函数
			(void *) 0,					//传递给任务函数的参数
			(OS_PRIO) BAT_TASK_PRIO,     //任务优先级
			(CPU_STK *) &BAT_TASK_STK[0],	//任务堆栈基地址
			(CPU_STK_SIZE) BAT_STK_SIZE / 10,	//任务堆栈深度限位
			(CPU_STK_SIZE) BAT_STK_SIZE,		//任务堆栈大小
			(OS_MSG_QTY) 0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
			(OS_TICK) 0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
			(void *) 0,					//用户补充的存储区
			(OS_OPT) OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR, //任务选项
			(OS_ERR *) &err);				//存放该函数错误时的返回值
	OSTaskCreate((OS_TCB *) &AnalyseTaskTCB,		//任务控制块
			(CPU_CHAR *) "analyse task", 		//任务名字
			(OS_TASK_PTR) analyse_task, 			//任务函数
			(void *) 0,					//传递给任务函数的参数
			(OS_PRIO) ANALYSE_TASK_PRIO,     //任务优先级
			(CPU_STK *) &ANALYSE_TASK_STK[0],	//任务堆栈基地址
			(CPU_STK_SIZE) ANALYSE_STK_SIZE / 10,	//任务堆栈深度限位
			(CPU_STK_SIZE) ANALYSE_STK_SIZE,		//任务堆栈大小
			(OS_MSG_QTY) 0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
			(OS_TICK) 0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
			(void *) 0,					//用户补充的存储区
			(OS_OPT) OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR, //任务选项
			(OS_ERR *) &err);				//存放该函数错误时的返回值
	OSTaskCreate((OS_TCB *) &Process_SeverTaskTCB,		//任务控制块
			(CPU_CHAR *) "process server task", 		//任务名字
			(OS_TASK_PTR) process_server_task, 			//任务函数
			(void *) 0,					//传递给任务函数的参数
			(OS_PRIO) PROCESS_SERVER_TASK_PRIO,     //任务优先级
			(CPU_STK *) &PROCESS_SERVER_TASK_STK[0],	//任务堆栈基地址
			(CPU_STK_SIZE) PROCESS_SERVER_STK_SIZE / 10,	//任务堆栈深度限位
			(CPU_STK_SIZE) PROCESS_SERVER_STK_SIZE,		//任务堆栈大小
			(OS_MSG_QTY) 4,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
			(OS_TICK) 0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
			(void *) 0,					//用户补充的存储区
			(OS_OPT) OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR, //任务选项
			(OS_ERR *) &err);			//存放该函数错误时的返回值
	OSTaskCreate((OS_TCB *) &NetworkTaskTCB,		//任务控制块
			(CPU_CHAR *) "network tasksk", 		//任务名字
			(OS_TASK_PTR) network_task, 			//任务函数
			(void *) 0,					//传递给任务函数的参数
			(OS_PRIO) NETWORK_TASK_PRIO,     //任务优先级
			(CPU_STK *) &NETWORK_TASK_STK[0],	//任务堆栈基地址
			(CPU_STK_SIZE) NETWORK_STK_SIZE / 10,	//任务堆栈深度限位
			(CPU_STK_SIZE) NETWORK_STK_SIZE,		//任务堆栈大小
			(OS_MSG_QTY) 4,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
			(OS_TICK) 0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
			(void *) 0,					//用户补充的存储区
			(OS_OPT) OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR, //任务选项
			(OS_ERR *) &err);			//存放该函数错误时的返回值
	OS_CRITICAL_EXIT()
	;	//退出临界区
#if PRINT_STK_USED
		OSTaskStkChk((OS_TCB *) 0, &n_free, &n_used, &err);
		DEBUG("StartTask::free=%u,used=%u\r\n", n_free, n_used);
#endif
	OSTaskDel((OS_TCB*) 0, &err);	//删除start_task任务自身
	myfree(START_TASK_STK);
}

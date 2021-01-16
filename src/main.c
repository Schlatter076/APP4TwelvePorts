#include "task.h"
#include "usart.h"
#include "L74HC595.h"
#include "mySwitch.h"
#include "motor.h"
#include "powerbank.h"
#include "backlight.h"
#include "F4G.h"
#include "wifi.h"

int main(void)
{
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x10000);
	__enable_irq(); //开启总中断
	delay_init(168);  	//时钟初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  	//中断分组配置
	my_mem_init();
	STMFlash_Init();
	USART1_Init(115200);
	AnalyzeServerParams();
	L74HC595_Init();
	Switch_Init();
	Motor_Init();
	Powerbank_Init(115200);
	Backlight_Init();
	F4G_Init(115200);
	WIFI_Init(115200);

#if SYSTEM_SUPPORT_OS
	OS_ERR err;
	CPU_SR_ALLOC();
	OSInit(&err);		    	//初始化UCOSIII
	OS_CRITICAL_ENTER()
	;	//进入临界区
	START_TASK_STK = mymalloc(START_STK_SIZE * 4);
	//创建开始任务
	OSTaskCreate((OS_TCB *) &StartTaskTCB,		//任务控制块
			(CPU_CHAR *) "start task", 		//任务名字
			(OS_TASK_PTR) start_task, 			//任务函数
			(void *) 0,					//传递给任务函数的参数
			(OS_PRIO) START_TASK_PRIO,     //任务优先级
			(CPU_STK *) &START_TASK_STK[0],	//任务堆栈基地址
			(CPU_STK_SIZE) START_STK_SIZE / 10,	//任务堆栈深度限位
			(CPU_STK_SIZE) START_STK_SIZE,		//任务堆栈大小
			(OS_MSG_QTY) 0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
			(OS_TICK) 0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
			(void *) 0,					//用户补充的存储区
			(OS_OPT) OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR, //任务选项
			(OS_ERR *) &err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT()
	;	//退出临界区
	OSStart(&err);      //开启UCOSIII
#endif
	while (1)
	{

	}
}

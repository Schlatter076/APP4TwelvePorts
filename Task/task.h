/*
 * task.h
 *
 *  Created on: 2021年1月5日
 *      Author: HHS007
 */

#ifndef TASK_H_
#define TASK_H_

#include "includes.h"
#include "malloc.h"
#include "mySwitch.h"
#include "STMFlash.h"
#include "powerbank.h"
#include "L74HC595.h"
#include "tcp_public.h"

//=========任务管理器===============================
//开始任务-用来创建其他任务,只运行一次
#define START_TASK_PRIO		3   			//任务优先级
#define START_STK_SIZE 		128  			//任务堆栈大小
OS_TCB StartTaskTCB;             			//任务控制块
extern u8 *START_TASK_STK;       			//任务堆栈
void start_task(void *p_arg);
//充电宝卡口检测任务
#define BAT_TASK_PRIO  4        			//任务优先级
#define BAT_STK_SIZE   128					//任务堆栈大小
OS_TCB  BatTaskTCB;							//任务控制块
CPU_STK BAT_TASK_STK[BAT_STK_SIZE];   		//任务堆栈
void bat_task(void *p_arg);
//卡口状态上报任务
#define REPORT_TASK_PRIO  5  				//任务优先级
#define REPORT_STK_SIZE   128  				//任务堆栈大小
OS_TCB  ReportTaskTCB;						//任务控制块
CPU_STK REPORT_TASK_STK[REPORT_STK_SIZE];   	//任务堆栈
void report_task(void *p_arg);
//串口1数据解析任务
#define USART1_TASK_PRIO  6  				//任务优先级
#define USART1_STK_SIZE   128  				//任务堆栈大小
OS_TCB  USART1TaskTCB;						//任务控制块
CPU_STK USART1_TASK_STK[REPORT_STK_SIZE];   	//任务堆栈
void usart1_task(void *p_arg);
//=========任务管理器===============================
//=========软件定时器===============================
OS_TMR 	Stmr1;
void Stmr1_callback(void *p_tmr, void *p_arg);
OS_TMR 	Stmr2;
void Stmr2_callback(void *p_tmr, void *p_arg);
OS_TMR 	Stmr3;
void Stmr3_callback(void *p_tmr, void *p_arg);
OS_TMR 	Stmr4;
void Stmr4_callback(void *p_tmr, void *p_arg);
//=========软件定时器===============================
//=========事件标志组===============================
/**
 * 定义一个事件标志组
 * bit0-0x01-串口6发送完成
 * bit1-0x02-串口6接收到数据
 * bit2-0x04-卡口检测完成标志
 * bit3-0x08-串口1发送完成
 * bit4-0x10-串口1接收到数据
 */
OS_FLAG_GRP	EventFlags;
//=========事件标志组===============================

#endif /* TASK_H_ */

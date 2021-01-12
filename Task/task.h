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
#include "F4G.h"
#include "wifi.h"
#include "base64.h"
#include "backlight.h"

//是否需要打印各任务堆栈使用情况
#define  PRINT_STK_USED    0u

//=========任务管理器===============================
//开始任务-用来创建其他任务,只运行一次
#define START_TASK_PRIO		3   			//任务优先级
#define START_STK_SIZE 		64  			//任务堆栈大小
OS_TCB StartTaskTCB;             			//任务控制块
extern u8 *START_TASK_STK;       			//任务堆栈
//CPU_STK START_TASK_STK[START_STK_SIZE];
void start_task(void *p_arg);
//数据解析任务
#define ANALYSE_TASK_PRIO  4  				//任务优先级
#define ANALYSE_STK_SIZE   192  				//任务堆栈大小
OS_TCB AnalyseTaskTCB;						//任务控制块
CPU_STK ANALYSE_TASK_STK[ANALYSE_STK_SIZE];   	//任务堆栈
void analyse_task(void *p_arg);
//服务器数据处理任务
#define PROCESS_SERVER_TASK_PRIO  5  				//任务优先级
#define PROCESS_SERVER_STK_SIZE   192  				//任务堆栈大小
OS_TCB Process_SeverTaskTCB;						//任务控制块
CPU_STK PROCESS_SERVER_TASK_STK[PROCESS_SERVER_STK_SIZE];   	//任务堆栈
void process_server_task(void *p_arg);
//联网任务
#define NETWORK_TASK_PRIO  6  				//任务优先级
#define NETWORK_STK_SIZE   192  				//任务堆栈大小
OS_TCB NetworkTaskTCB;						//任务控制块
CPU_STK NETWORK_TASK_STK[NETWORK_STK_SIZE];   	//任务堆栈
void network_task(void *p_arg);
//充电宝卡口检测任务
#define BAT_TASK_PRIO  7        			//任务优先级
#define BAT_STK_SIZE   192					//任务堆栈大小
OS_TCB BatTaskTCB;							//任务控制块
CPU_STK BAT_TASK_STK[BAT_STK_SIZE];   		//任务堆栈
void bat_task(void *p_arg);
//=========任务管理器===============================
//=========软件定时器===============================
OS_TMR Stmr3;
void Stmr3_callback(void *p_tmr, void *p_arg);
OS_TMR Stmr4;
void Stmr4_callback(void *p_tmr, void *p_arg);
//=========软件定时器===============================
//=========事件标志组===============================
#define FLAG_USART6_TxED  		((OS_FLAGS)0x01)
#define FLAG_USART6_RxED  		((OS_FLAGS)0x02)
#define FLAG_SCAN_COMP    		((OS_FLAGS)0x04)
#define FLAG_USART1_TxED  		((OS_FLAGS)0x08)
#define FLAG_USART1_RxED  		((OS_FLAGS)0x10)
#define FLAG_USART3_TxED  		((OS_FLAGS)0x20)
#define FLAG_USART3_RxED  		((OS_FLAGS)0x40)
#define FLAG_USART2_TxED  		((OS_FLAGS)0x80)
#define FLAG_USART2_RxED  		((OS_FLAGS)0x0100)
#define FLAG_WIFI_AT      		((OS_FLAGS)0x0200)
#define FLAG_F4G_AT       		((OS_FLAGS)0x0400)
#define FLAG_COMMON_HEART       ((OS_FLAGS)0x0800)
#define FLAG_STATU_HEART        ((OS_FLAGS)0x1000)
#define FLAG_F4G_FIR_REP        ((OS_FLAGS)0x2000)
#define FLAG_WIFI_FIR_REP       ((OS_FLAGS)0x4000)
OS_FLAG_GRP EventFlags;
//=========事件标志组===============================

#endif /* TASK_H_ */

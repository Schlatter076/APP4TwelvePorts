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

//任务优先级
#define START_TASK_PRIO		3
//任务堆栈大小
#define START_STK_SIZE 		128
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈
extern u8 *START_TASK_STK;

void start_task(void *p_arg);

#endif /* TASK_H_ */

/*
 * motor.h
 *
 *  Created on: 2021年1月5日
 *      Author: HHS007
 */

#ifndef MOTOR_MOTOR_H_
#define MOTOR_MOTOR_H_

#include "sys.h"
#include "delay.h"
#include "tcp_public.h"

#define FOREWARD  1
#define BACKWARD  0

void Motor_Init(void);
void Motor_run(u8 mot, u8 dir);
void remoteCtrMotot(u8 powerBank, u16 gtime, u16 ktime);
void popUP_All(void);
void popUP_powerBank(u8 powerBank);
void Motor_stop(u8 mot);

#endif /* MOTOR_MOTOR_H_ */

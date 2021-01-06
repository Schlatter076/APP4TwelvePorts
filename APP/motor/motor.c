/*
 * motor.c
 *
 *  Created on: 2021年1月5日
 *      Author: HHS007
 */
#include "motor.h"

void Motor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_14
			| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_10
			| GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	//全部电机停止
	PAout(11) = 0;
	PAout(12) = 0;
	PCout(10) = 0;
	PCout(11) = 0;
	PCout(14) = 0;
	PCout(15) = 0;
	PDout(2) = 0;
	PDout(3) = 0;
	PDout(10) = 0;
	PDout(11) = 0;
	PEout(2) = 0;
	PEout(3) = 0;
}
/**
 * 控制电机转
 * @mot 1-6
 * @dir FOREWARD BACKWARD
 */
void Motor_run(u8 mot, u8 dir)
{
	switch (mot)
	{
	case 1:
		PCout(11) = dir;
		PCout(10) = (dir == FOREWARD) ? 0 : 1;
		break;
	case 2:
		PDout(3) = dir;
		PDout(2) = (dir == FOREWARD) ? 0 : 1;
		break;
	case 3:
		PAout(11) = dir;
		PAout(12) = (dir == FOREWARD) ? 0 : 1;
		break;
	case 4:
		PEout(3) = dir;
		PEout(2) = (dir == FOREWARD) ? 0 : 1;
		break;
	case 5:
		PDout(10) = dir;
		PDout(11) = (dir == FOREWARD) ? 0 : 1;
		break;
	case 6:
		PCout(15) = dir;
		PCout(14) = (dir == FOREWARD) ? 0 : 1;
		break;
	default:
		break;
	}
}
/**
 * 电机停止转动
 * @mot 1-6
 */
void Motor_stop(u8 mot)
{
	switch (mot)
	{
	case 1:
		PCout(11) = 0;
		PCout(10) = 0;
		break;
	case 2:
		PDout(3) = 0;
		PDout(2) = 0;
		break;
	case 3:
		PAout(11) = 0;
		PAout(12) = 0;
		break;
	case 4:
		PEout(3) = 0;
		PEout(2) = 0;
		break;
	case 5:
		PDout(10) = 0;
		PDout(11) = 0;
		break;
	case 6:
		PCout(15) = 0;
		PCout(14) = 0;
		break;
	default:
		break;
	}
}

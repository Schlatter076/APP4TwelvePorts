/*
 * mySwitch.c
 *
 *  Created on: 2021年1月5日
 *      Author: HHS007
 */
#include "mySwitch.h"

void Switch_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5
			| GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_9
			| GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_12
			| GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_6
			| GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}
/**
 * 获取充电宝检测开关
 * @bat 1-12
 * @return 状态 0表示压住 1表示弹开
 */
u8 getBatSwitchSTA(u8 bat)
{
	u8 res = 0;
	switch (bat)
	{
	case 1:
		res = PBin(6);
		break;
	case 2:
		res = PBin(5);
		break;
	case 3:
		res = PBin(8);
		break;
	case 4:
		res = PBin(9);
		break;
	case 5:
		res = PDin(13);
		break;
	case 6:
		res = PDin(12);
		break;
	case 7:
		res = PCin(13);
		break;
	case 8:
		res = PEin(6);
		break;
	case 9:
		res = PBin(1);
		break;
	case 10:
		res = PBin(0);
		break;
	case 11:
		res = PCin(1);
		break;
	case 12:
		res = PCin(0);
		break;
	default:
		break;
	}
	return res == 0 ? 1 : 0;
}

/**
 * 获取电机检测开关
 * @bat 1-12
 * @return 状态 0表示压住 1表示弹开
 */
u8 getMotorSwitchSTA(u8 mot)
{
	u8 res = 0;
	switch (mot)
	{
	case 1:
		res = PAin(15);
		break;
	case 2:
		res = PCin(12);
		break;
	case 3:
		res = PDin(0);
		break;
	case 4:
		res = PDin(1);
		break;
	case 5:
		res = PCin(9);
		break;
	case 6:
		res = PAin(8);
		break;
	case 7:
		res = PEin(0);
		break;
	case 8:
		res = PEin(1);
		break;
	case 9:
		res = PEin(12);
		break;
	case 10:
		res = PEin(13);
		break;
	case 11:
		res = PEin(10);
		break;
	case 12:
		res = PEin(11);
		break;
	default:
		break;
	}
	return res == 0 ? 1 : 0;
}


/*
 * charge.c
 *
 *  Created on: 2021年1月6日
 *      Author: HHS007
 */
#include "powerbank.h"

void Charge_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6
			| GPIO_Pin_7 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7
			| GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/**
 * 控制充电宝充电函数
 * @pb 1-12表示充电宝1-12
 * @allow 1-表示允许  0-表示不允许
 */
void controlPowerBankCharge(u8 pb, u8 allow)
{
	switch (pb)
	{
	case 1:
		BAT_EN1 = allow;
		break;
	case 2:
		BAT_EN2 = allow;
		break;
	case 3:
		BAT_EN3 = allow;
		break;
	case 4:
		BAT_EN4 = allow;
		break;
	case 5:
		BAT_EN5 = allow;
		break;
	case 6:
		BAT_EN6 = allow;
		break;
	case 7:
		BAT_EN7 = allow;
		break;
	case 8:
		BAT_EN8 = allow;
		break;
	case 9:
		BAT_EN9 = allow;
		break;
	case 10:
		BAT_EN10 = allow;
		break;
	case 11:
		BAT_EN11 = allow;
		break;
	case 12:
		BAT_EN12 = allow;
		break;
	default:
		break;
	}
}

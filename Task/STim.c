/*
 * STim.c
 *
 *  Created on: 2021年1月7日
 *      Author: HHS007
 */
#include "task.h"

/**
 * 软件定时器1
 * 用来4G模块第一次上报卡口状态失败
 * 初始延时20S  周期延时20S
 */
void Stmr1_callback(void *p_tmr, void *p_arg)
{
	forceHeart(In4G, UP_AllPortsSTA);
}
/**
 * 软件定时器2
 * 用来Wifi模块第一次上报卡口状态失败
 * 初始延时20S  周期延时20S
 */
void Stmr2_callback(void *p_tmr, void *p_arg)
{
	forceHeart(InWifi, UP_AllPortsSTA);
}
/**
 * 软件定时器3
 * 用来3S以后允许同一卡口弹出充电宝
 * 初始延时3S 单次定时器
 */
void Stmr3_callback(void *p_tmr, void *p_arg)
{
	curPort = 0xFF;
}
/**
 * 软件定时器4
 * 用来控制卡口状态灯
 * 无初始延时  周期延时100ms
 */
void Stmr4_callback(void *p_tmr, void *p_arg)
{
	//快闪
	if (HC595_STATUS.fastBLINK[0])
	{
		HC595_STATUS.fastCnt[0]++;
		if (HC595_STATUS.fastCnt[0] == 20)
		{
			HC595_STATUS.fastCnt[0] = 0;
			ledBLINK(1);
		}
	}
	//慢闪
	if (HC595_STATUS.slowBLINK[0])
	{
		HC595_STATUS.slowCnt[0]++;
		if (HC595_STATUS.slowCnt[0] == 80)
		{
			HC595_STATUS.slowCnt[0] = 0;
			ledBLINK(1);
		}
	}
	//快闪
	if (HC595_STATUS.fastBLINK[1])
	{
		HC595_STATUS.fastCnt[1]++;
		if (HC595_STATUS.fastCnt[1] == 20)
		{
			HC595_STATUS.fastCnt[1] = 0;
			ledBLINK(2);
		}
	}
	//慢闪
	if (HC595_STATUS.slowBLINK[1])
	{
		HC595_STATUS.slowCnt[1]++;
		if (HC595_STATUS.slowCnt[1] == 80)
		{
			HC595_STATUS.slowCnt[1] = 0;
			ledBLINK(2);
		}
	}
	//快闪
	if (HC595_STATUS.fastBLINK[2])
	{
		HC595_STATUS.fastCnt[2]++;
		if (HC595_STATUS.fastCnt[2] == 20)
		{
			HC595_STATUS.fastCnt[2] = 0;
			ledBLINK(3);
		}
	}
	//慢闪
	if (HC595_STATUS.slowBLINK[2])
	{
		HC595_STATUS.slowCnt[2]++;
		if (HC595_STATUS.slowCnt[2] == 80)
		{
			HC595_STATUS.slowCnt[2] = 0;
			ledBLINK(3);
		}
	}
	//快闪
	if (HC595_STATUS.fastBLINK[3])
	{
		HC595_STATUS.fastCnt[3]++;
		if (HC595_STATUS.fastCnt[3] == 20)
		{
			HC595_STATUS.fastCnt[3] = 0;
			ledBLINK(4);
		}
	}
	//慢闪
	if (HC595_STATUS.slowBLINK[3])
	{
		HC595_STATUS.slowCnt[3]++;
		if (HC595_STATUS.slowCnt[3] == 80)
		{
			HC595_STATUS.slowCnt[3] = 0;
			ledBLINK(4);
		}
	}
	//快闪
	if (HC595_STATUS.fastBLINK[4])
	{
		HC595_STATUS.fastCnt[4]++;
		if (HC595_STATUS.fastCnt[4] == 20)
		{
			HC595_STATUS.fastCnt[4] = 0;
			ledBLINK(5);
		}
	}
	//慢闪
	if (HC595_STATUS.slowBLINK[4])
	{
		HC595_STATUS.slowCnt[4]++;
		if (HC595_STATUS.slowCnt[4] == 80)
		{
			HC595_STATUS.slowCnt[4] = 0;
			ledBLINK(5);
		}
	}
	//快闪
	if (HC595_STATUS.fastBLINK[5])
	{
		HC595_STATUS.fastCnt[5]++;
		if (HC595_STATUS.fastCnt[5] == 20)
		{
			HC595_STATUS.fastCnt[5] = 0;
			ledBLINK(6);
		}
	}
	//慢闪
	if (HC595_STATUS.slowBLINK[5])
	{
		HC595_STATUS.slowCnt[5]++;
		if (HC595_STATUS.slowCnt[5] == 80)
		{
			HC595_STATUS.slowCnt[5] = 0;
			ledBLINK(6);
		}
	}
	//快闪
	if (HC595_STATUS.fastBLINK[6])
	{
		HC595_STATUS.fastCnt[6]++;
		if (HC595_STATUS.fastCnt[6] == 20)
		{
			HC595_STATUS.fastCnt[6] = 0;
			ledBLINK(7);
		}
	}
	//慢闪
	if (HC595_STATUS.slowBLINK[6])
	{
		HC595_STATUS.slowCnt[6]++;
		if (HC595_STATUS.slowCnt[6] == 80)
		{
			HC595_STATUS.slowCnt[6] = 0;
			ledBLINK(7);
		}
	}
	//快闪
	if (HC595_STATUS.fastBLINK[7])
	{
		HC595_STATUS.fastCnt[7]++;
		if (HC595_STATUS.fastCnt[7] == 20)
		{
			HC595_STATUS.fastCnt[7] = 0;
			ledBLINK(8);
		}
	}
	//慢闪
	if (HC595_STATUS.slowBLINK[7])
	{
		HC595_STATUS.slowCnt[7]++;
		if (HC595_STATUS.slowCnt[7] == 80)
		{
			HC595_STATUS.slowCnt[7] = 0;
			ledBLINK(8);
		}
	}
	//快闪
	if (HC595_STATUS.fastBLINK[8])
	{
		HC595_STATUS.fastCnt[8]++;
		if (HC595_STATUS.fastCnt[8] == 20)
		{
			HC595_STATUS.fastCnt[8] = 0;
			ledBLINK(9);
		}
	}
	//慢闪
	if (HC595_STATUS.slowBLINK[8])
	{
		HC595_STATUS.slowCnt[8]++;
		if (HC595_STATUS.slowCnt[8] == 80)
		{
			HC595_STATUS.slowCnt[8] = 0;
			ledBLINK(9);
		}
	}
	//快闪
	if (HC595_STATUS.fastBLINK[9])
	{
		HC595_STATUS.fastCnt[9]++;
		if (HC595_STATUS.fastCnt[9] == 20)
		{
			HC595_STATUS.fastCnt[9] = 0;
			ledBLINK(10);
		}
	}
	//慢闪
	if (HC595_STATUS.slowBLINK[9])
	{
		HC595_STATUS.slowCnt[9]++;
		if (HC595_STATUS.slowCnt[9] == 80)
		{
			HC595_STATUS.slowCnt[9] = 0;
			ledBLINK(10);
		}
	}
	//快闪
	if (HC595_STATUS.fastBLINK[10])
	{
		HC595_STATUS.fastCnt[10]++;
		if (HC595_STATUS.fastCnt[10] == 20)
		{
			HC595_STATUS.fastCnt[10] = 0;
			ledBLINK(11);
		}
	}
	//慢闪
	if (HC595_STATUS.slowBLINK[10])
	{
		HC595_STATUS.slowCnt[10]++;
		if (HC595_STATUS.slowCnt[10] == 80)
		{
			HC595_STATUS.slowCnt[10] = 0;
			ledBLINK(11);
		}
	}
	//快闪
	if (HC595_STATUS.fastBLINK[11])
	{
		HC595_STATUS.fastCnt[11]++;
		if (HC595_STATUS.fastCnt[11] == 20)
		{
			HC595_STATUS.fastCnt[11] = 0;
			ledBLINK(12);
		}
	}
	//慢闪
	if (HC595_STATUS.slowBLINK[11])
	{
		HC595_STATUS.slowCnt[11]++;
		if (HC595_STATUS.slowCnt[11] == 80)
		{
			HC595_STATUS.slowCnt[11] = 0;
			ledBLINK(12);
		}
	}
}

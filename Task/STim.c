/*
 * STim.c
 *
 *  Created on: 2021年1月7日
 *      Author: HHS007
 */
#include "task.h"

volatile u8 initCnt = 0;
volatile u16 heartCnt = 0;
volatile u8 statuHeartCnt = 0;
volatile u16 firstStatuHeartCnt = 0;
volatile u16 wifi_firstStatuHeartCnt = 0;

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
	OS_ERR err;
	//未上线
	if (F4G_Fram.Online == 0 && WIFI_Fram.Online == 0)
	{
		if (initCnt++ == 5)
		{
			initCnt = 0;
			BACK_LIGHT_STA ^= 1;
		}
	}
	//第一次上报口状态失败
	if (F4G_Fram.firstStatuHeartNotSucc)
	{
		if (firstStatuHeartCnt++ == 200) //20S
		{
			firstStatuHeartCnt = 0;
			OSFlagPost((OS_FLAG_GRP*) &EventFlags, //对应的事件标志组
					(OS_FLAGS) FLAG_F4G_FIR_REP, //事件位
					(OS_OPT) OS_OPT_POST_FLAG_SET, //选择置位
					(OS_ERR*) &err); //错误码
		}
	}
	if (WIFI_Fram.firstStatuHeartNotSucc)
	{
		if (wifi_firstStatuHeartCnt++ == 200) //20S
		{
			wifi_firstStatuHeartCnt = 0;
			OSFlagPost((OS_FLAG_GRP*) &EventFlags, //对应的事件标志组
					(OS_FLAGS) FLAG_WIFI_FIR_REP, //事件位
					(OS_OPT) OS_OPT_POST_FLAG_SET, //选择置位
					(OS_ERR*) &err); //错误码
		}
	}
	if (F4G_Fram.allowHeart || WIFI_Fram.allowHeart)
	{
		heartCnt++;
		if (heartCnt
				== RegisterParams.heartTime * 10
						/ RegisterParams.statuHeartTime)  //达到心跳时间一半时检查连接
		{
			OSFlagPost((OS_FLAG_GRP*) &EventFlags, //对应的事件标志组
					(OS_FLAGS) FLAG_CHECK_LINKS, //事件位
					(OS_OPT) OS_OPT_POST_FLAG_SET, //选择置位
					(OS_ERR*) &err); //错误码
		}
		else if (heartCnt == RegisterParams.heartTime * 10)
		{
			heartCnt = 0;
			if (statuHeartCnt++ == RegisterParams.statuHeartTime)
			{
				statuHeartCnt = 0;
				OSFlagPost((OS_FLAG_GRP*) &EventFlags, //对应的事件标志组
						(OS_FLAGS) FLAG_STATU_HEART, //事件位
						(OS_OPT) OS_OPT_POST_FLAG_SET, //选择置位
						(OS_ERR*) &err); //错误码
			}
			else //状态心跳的时候不传普通心跳
			{
				OSFlagPost((OS_FLAG_GRP*) &EventFlags, //对应的事件标志组
						(OS_FLAGS) FLAG_COMMON_HEART, //事件位
						(OS_OPT) OS_OPT_POST_FLAG_SET, //选择置位
						(OS_ERR*) &err); //错误码
			}
		}
	}
	//快闪
	if (HC595_STATUS.fastBLINK[0])
	{
		HC595_STATUS.fastCnt[0]++;
		if (HC595_STATUS.fastCnt[0] == 2)
		{
			HC595_STATUS.fastCnt[0] = 0;
			ledBLINK(1);
		}
	}
	//慢闪
	if (HC595_STATUS.slowBLINK[0])
	{
		HC595_STATUS.slowCnt[0]++;
		if (HC595_STATUS.slowCnt[0] == 8)
		{
			HC595_STATUS.slowCnt[0] = 0;
			ledBLINK(1);
		}
	}
	//快闪
	if (HC595_STATUS.fastBLINK[1])
	{
		HC595_STATUS.fastCnt[1]++;
		if (HC595_STATUS.fastCnt[1] == 2)
		{
			HC595_STATUS.fastCnt[1] = 0;
			ledBLINK(2);
		}
	}
	//慢闪
	if (HC595_STATUS.slowBLINK[1])
	{
		HC595_STATUS.slowCnt[1]++;
		if (HC595_STATUS.slowCnt[1] == 8)
		{
			HC595_STATUS.slowCnt[1] = 0;
			ledBLINK(2);
		}
	}
	//快闪
	if (HC595_STATUS.fastBLINK[2])
	{
		HC595_STATUS.fastCnt[2]++;
		if (HC595_STATUS.fastCnt[2] == 2)
		{
			HC595_STATUS.fastCnt[2] = 0;
			ledBLINK(3);
		}
	}
	//慢闪
	if (HC595_STATUS.slowBLINK[2])
	{
		HC595_STATUS.slowCnt[2]++;
		if (HC595_STATUS.slowCnt[2] == 8)
		{
			HC595_STATUS.slowCnt[2] = 0;
			ledBLINK(3);
		}
	}
	//快闪
	if (HC595_STATUS.fastBLINK[3])
	{
		HC595_STATUS.fastCnt[3]++;
		if (HC595_STATUS.fastCnt[3] == 2)
		{
			HC595_STATUS.fastCnt[3] = 0;
			ledBLINK(4);
		}
	}
	//慢闪
	if (HC595_STATUS.slowBLINK[3])
	{
		HC595_STATUS.slowCnt[3]++;
		if (HC595_STATUS.slowCnt[3] == 8)
		{
			HC595_STATUS.slowCnt[3] = 0;
			ledBLINK(4);
		}
	}
	//快闪
	if (HC595_STATUS.fastBLINK[4])
	{
		HC595_STATUS.fastCnt[4]++;
		if (HC595_STATUS.fastCnt[4] == 2)
		{
			HC595_STATUS.fastCnt[4] = 0;
			ledBLINK(5);
		}
	}
	//慢闪
	if (HC595_STATUS.slowBLINK[4])
	{
		HC595_STATUS.slowCnt[4]++;
		if (HC595_STATUS.slowCnt[4] == 8)
		{
			HC595_STATUS.slowCnt[4] = 0;
			ledBLINK(5);
		}
	}
	//快闪
	if (HC595_STATUS.fastBLINK[5])
	{
		HC595_STATUS.fastCnt[5]++;
		if (HC595_STATUS.fastCnt[5] == 2)
		{
			HC595_STATUS.fastCnt[5] = 0;
			ledBLINK(6);
		}
	}
	//慢闪
	if (HC595_STATUS.slowBLINK[5])
	{
		HC595_STATUS.slowCnt[5]++;
		if (HC595_STATUS.slowCnt[5] == 8)
		{
			HC595_STATUS.slowCnt[5] = 0;
			ledBLINK(6);
		}
	}
	//快闪
	if (HC595_STATUS.fastBLINK[6])
	{
		HC595_STATUS.fastCnt[6]++;
		if (HC595_STATUS.fastCnt[6] == 2)
		{
			HC595_STATUS.fastCnt[6] = 0;
			ledBLINK(7);
		}
	}
	//慢闪
	if (HC595_STATUS.slowBLINK[6])
	{
		HC595_STATUS.slowCnt[6]++;
		if (HC595_STATUS.slowCnt[6] == 8)
		{
			HC595_STATUS.slowCnt[6] = 0;
			ledBLINK(7);
		}
	}
	//快闪
	if (HC595_STATUS.fastBLINK[7])
	{
		HC595_STATUS.fastCnt[7]++;
		if (HC595_STATUS.fastCnt[7] == 2)
		{
			HC595_STATUS.fastCnt[7] = 0;
			ledBLINK(8);
		}
	}
	//慢闪
	if (HC595_STATUS.slowBLINK[7])
	{
		HC595_STATUS.slowCnt[7]++;
		if (HC595_STATUS.slowCnt[7] == 8)
		{
			HC595_STATUS.slowCnt[7] = 0;
			ledBLINK(8);
		}
	}
	//快闪
	if (HC595_STATUS.fastBLINK[8])
	{
		HC595_STATUS.fastCnt[8]++;
		if (HC595_STATUS.fastCnt[8] == 2)
		{
			HC595_STATUS.fastCnt[8] = 0;
			ledBLINK(9);
		}
	}
	//慢闪
	if (HC595_STATUS.slowBLINK[8])
	{
		HC595_STATUS.slowCnt[8]++;
		if (HC595_STATUS.slowCnt[8] == 8)
		{
			HC595_STATUS.slowCnt[8] = 0;
			ledBLINK(9);
		}
	}
	//快闪
	if (HC595_STATUS.fastBLINK[9])
	{
		HC595_STATUS.fastCnt[9]++;
		if (HC595_STATUS.fastCnt[9] == 2)
		{
			HC595_STATUS.fastCnt[9] = 0;
			ledBLINK(10);
		}
	}
	//慢闪
	if (HC595_STATUS.slowBLINK[9])
	{
		HC595_STATUS.slowCnt[9]++;
		if (HC595_STATUS.slowCnt[9] == 8)
		{
			HC595_STATUS.slowCnt[9] = 0;
			ledBLINK(10);
		}
	}
	//快闪
	if (HC595_STATUS.fastBLINK[10])
	{
		HC595_STATUS.fastCnt[10]++;
		if (HC595_STATUS.fastCnt[10] == 2)
		{
			HC595_STATUS.fastCnt[10] = 0;
			ledBLINK(11);
		}
	}
	//慢闪
	if (HC595_STATUS.slowBLINK[10])
	{
		HC595_STATUS.slowCnt[10]++;
		if (HC595_STATUS.slowCnt[10] == 8)
		{
			HC595_STATUS.slowCnt[10] = 0;
			ledBLINK(11);
		}
	}
	//快闪
	if (HC595_STATUS.fastBLINK[11])
	{
		HC595_STATUS.fastCnt[11]++;
		if (HC595_STATUS.fastCnt[11] == 2)
		{
			HC595_STATUS.fastCnt[11] = 0;
			ledBLINK(12);
		}
	}
	//慢闪
	if (HC595_STATUS.slowBLINK[11])
	{
		HC595_STATUS.slowCnt[11]++;
		if (HC595_STATUS.slowCnt[11] == 8)
		{
			HC595_STATUS.slowCnt[11] = 0;
			ledBLINK(12);
		}
	}
}

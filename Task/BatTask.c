/*
 * BatTask.c
 *
 *  Created on: 2021年1月7日
 *      Author: HHS007
 */
#include "task.h"

void chooseFlagToRun(OS_FLAGS f);

void bat_task(void *p_arg)
{
	OS_ERR err;
	u8 portSTA = 0;
	p_arg = p_arg;
	OS_FLAGS flag;
#if PRINT_STK_USED
	CPU_STK_SIZE n_free;
	CPU_STK_SIZE n_used;
#endif
	while (1)
	{
		//检测所有卡口充电宝的状态
		for (int i = 0; i < 12; i++)
		{
			portSTA = checkPowerbankStatus(i);
			if (portSTA != PowerbankSTA.currentStatuCode[i]) //卡口状态发生变化了
			{
				PowerbankSTA.currentStatuCode[i] = portSTA;
				//上报卡口状态
				if (F4G_Fram.allowHeart)
				{
					reportPortStatuChanged(In4G, i);
				}
				OSTimeDlyHMSM(0, 0, 0, 200, OS_OPT_TIME_PERIODIC, &err);
				if (WIFI_Fram.allowHeart)
				{
					reportPortStatuChanged(InWifi, i);
				}
			}
		}
		//从这里取充电宝状态值
		flag = OSFlagPend(
				(OS_FLAG_GRP*) &EventFlags,  //事件标志组
				(OS_FLAGS) FLAG_COMMON_HEART + FLAG_STATU_HEART
						+ FLAG_F4G_FIR_REP + FLAG_WIFI_FIR_REP, //事件位
				(OS_TICK) 100,    //超时时间-等500ms
				(OS_OPT) OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME, //等待置位并清除
				(CPU_TS*) 0,    //时间戳
				(OS_ERR*) &err); //错误码

		if (err == OS_OPT_NONE) //不是由于超时等待
		{
			chooseFlagToRun(flag);
		}

#if PRINT_STK_USED
		OSTaskStkChk((OS_TCB *) 0, &n_free, &n_used, &err);
		DEBUG("BatTaskTCB::free=%u,used=%u\r\n", n_free, n_used);
#endif
		//每200ms检测一次卡口状态
		OSTimeDlyHMSM(0, 0, 0, 200, OS_OPT_TIME_PERIODIC, &err);
	}
}

void chooseFlagToRun(OS_FLAGS f)
{
	OS_ERR err;
	switch (f)
	{
	case FLAG_COMMON_HEART:
		if (F4G_Fram.allowHeart)
		{
			commonHeart(In4G);
		}
		OSTimeDlyHMSM(0, 0, 0, 200, OS_OPT_TIME_PERIODIC, &err);
		if (WIFI_Fram.allowHeart)
		{
			commonHeart(InWifi);
		}
		break;
	case FLAG_STATU_HEART:
		if (F4G_Fram.allowHeart)
		{
			forceHeart(In4G, UP_StatuHeart, DISABLE);
		}
		OSTimeDlyHMSM(0, 0, 0, 200, OS_OPT_TIME_PERIODIC, &err);
		if (WIFI_Fram.allowHeart)
		{
			forceHeart(InWifi, UP_StatuHeart, DISABLE);
		}
		break;
	case FLAG_F4G_FIR_REP:
		forceHeart(In4G, UP_AllPortsSTA, DISABLE);
		break;
	case FLAG_WIFI_FIR_REP:
		forceHeart(InWifi, UP_AllPortsSTA, DISABLE);
		break;
	default:
		break;
	}
}

/*
 * BatTask.c
 *
 *  Created on: 2021年1月7日
 *      Author: HHS007
 */
#include "task.h"

void bat_task(void *p_arg)
{
	OS_ERR err;
	u8 portSTA = 0;
	p_arg = p_arg;
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
				_USART_Printf(InUsart1, "%s,%d,%s-%s", MyFlashParams.DeviceID,
						UP_PowerbankSTAChanged,
						PowerbankSTA.powerBankBuf[i], "aaaaa");
				if (F4G_Fram.Online)
				{

				}
				if (WIFI_Fram.Online)
				{

				}
			}
		}
		//每200ms检测一次卡口状态
		OSTimeDlyHMSM(0, 0, 0, 200, OS_OPT_TIME_PERIODIC, &err);
	}
}

/*
 * NetworkTask.c
 *
 *  Created on: 2021年1月8日
 *      Author: HHS007
 */
#include "task.h"

void network_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while (1)
	{
		//4G模块还没有上电
		if (!F4G_Fram.AT_test_OK)
		{
			_F4G_PKEY = 1;
			delay_ms(1000);
			delay_ms(1000);
			delay_ms(1000);
			delay_ms(1000);
			_F4G_PKEY = 0;
			//复位4G模块
			_F4G_RST = 1;
			delay_ms(1100);
			_F4G_RST = 0;
			delay_ms(500);
			F4G_Fram.AT_test_OK = AT_Test(In4G);
			if(F4G_Fram.AT_test_OK)
			{
				getModuleMes();
			}
		}
		else
		{
			if(!F4G_Fram.Online)
			{
				F4G_Fram.Online = ConnectToServerBy4G(RegisterParams.ip, RegisterParams.port);
			}
		}

		OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TIME_PERIODIC, &err); //延时1S

	}
}


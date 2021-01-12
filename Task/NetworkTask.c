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
#if PRINT_STK_USED
	CPU_STK_SIZE n_free;
	CPU_STK_SIZE n_used;
#endif
	while (1)
	{
		//1.检查模块是否正常工作
		if (!F4G_Fram.AT_test_OK)
		{
			//2.检查模块是否需要重新开关机
			F4G_Fram.AT_test_OK = AT_Test(In4G);
			if (!F4G_Fram.AT_test_OK)
			{
				//3.执行开关机操作
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
				//4.再次检查模块是否正常工作
				F4G_Fram.AT_test_OK = AT_Test(In4G);
			}
			//5.如果模块已经正常工作
			if (F4G_Fram.AT_test_OK)
			{
				//6.获取模块相关的一些参数
				getModuleMes();
				//7.执行TCP连接
				F4G_Fram.Online = ConnectToServerBy4G(RegisterParams.ip,
						RegisterParams.port);
				//8.如果模块已经成功连接到ip和端口
				if (F4G_Fram.Online)
				{
					//9.执行注册请求
					request4Register(In4G);
				}
			}
		}
		//10.模块处于正常工作状态
		else
		{
			//11.如果第一次没有成功连接到ip或模块丢失掉连接
			if (!F4G_Fram.Online)
			{
				//12.再次执行ip连接
				F4G_Fram.Online = ConnectToServerBy4G(RegisterParams.ip,
						RegisterParams.port);

				//13.如果成功连接到ip
				if (F4G_Fram.Online)
				{
					//14.再次执行注册请求
					request4Register(In4G);
				}
			}
			//15.如果成功连接到ip，但没有注册成功
			else if (!F4G_Fram.allowHeart)
			{
				//16.再次执行注册请求
				request4Register(In4G);
			}
		}
		//wifi上电================================================
		//1.检查模块是否正常工作
		if (!WIFI_Fram.AT_test_OK)
		{
			//2.检查模块是否需要重新开关机
			WIFI_Fram.AT_test_OK = AT_Test(InWifi);
			if (!WIFI_Fram.AT_test_OK)
			{
				//3.执行开关机操作
				_WIFI_RST = 0;
				delay_ms(1000);
				delay_ms(1000);
				_WIFI_RST = 1;
				delay_ms(100);
				//4.再次检查模块是否正常工作
				WIFI_Fram.AT_test_OK = AT_Test(InWifi);
			}
			//5.如果模块已经正常工作,且配置过wifi参数
			if (WIFI_Fram.AT_test_OK && MyFlashParams.WifiFlag == WIFI_FLAG)
			{
				//6.执行TCP连接
				WIFI_Fram.Online = ConnectToServerByWIFI(RegisterParams.ip,
						RegisterParams.port);
				//7.如果模块已经成功连接到ip和端口
				if (WIFI_Fram.Online)
				{
					//8.执行注册请求
					request4Register(InWifi);
				}
			}
		}
		//9.模块处于正常工作状态
		else
		{
			//10.如果第一次没有成功连接到ip或模块丢失掉连接或wifi配置被修改
			if (!WIFI_Fram.Online || TCP_Params.wifiParamModified)
			{
				//11.再次执行ip连接
				WIFI_Fram.Online = ConnectToServerByWIFI(RegisterParams.ip,
						RegisterParams.port);

				//12.如果成功连接到ip
				if (WIFI_Fram.Online)
				{
					//13.清除wifi参数更改标志
					TCP_Params.wifiParamModified = 0;
					//14.再次执行注册请求
					request4Register(InWifi);
				}
			}
			//15.如果成功连接到ip，但没有注册成功
			else if (!WIFI_Fram.allowHeart)
			{
				//16.再次执行注册请求
				request4Register(InWifi);
			}
		}

#if PRINT_STK_USED
		OSTaskStkChk((OS_TCB *) 0, &n_free, &n_used, &err);
		DEBUG("NetworkTask::free=%u,used=%u\r\n", n_free, n_used);
#endif
		OSTimeDlyHMSM(0, 0, 2, 0, OS_OPT_TIME_PERIODIC, &err); //延时2S
	}
}


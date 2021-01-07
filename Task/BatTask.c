/*
 * BatTask.c
 *
 *  Created on: 2021年1月7日
 *      Author: HHS007
 */
#include "task.h"

/**卡口状态*/
const unsigned char bat_statuBuf[16] =
{ 1, 0, 9, 8, 3, 2, 11, 10, 0xFF, 0xFF, 5, 4, 0xFF, 0xFF, 7, 6 };

void bat_task(void *p_arg)
{
	u8 pbStatu = 0;
	u8 motorStatu = 0;
	u8 communicationStatu = 0;
	u8 index = 0;
	u8 pbERROR = 0;
	u16 len = 0;
	OS_ERR err;
	p_arg = p_arg;
	len = fillDataToTxBuf(BAT_Down_Admin, 0, NULL);
	while (1)
	{
		//检测所有卡口充电宝的状态
		for (int i = 0; i < 12; i++)
		{
			pbStatu = getBatSwitchSTA(i + 1);
			motorStatu = getMotorSwitchSTA(i + 1);
			//如果当前卡口未通电,则先通电
			if (!PowerbankSTA.Charging[i])
			{
				controlPowerBankCharge(i + 1, 1);
			}
			_USART6_CTL = 1; //串口6发送使能
			communicateWithPort(i + 1); //接通对应串口通信线
			USART6_DMA_Send(len); //向充电宝发送数据
			//等待数据发送完成
			OSFlagPend(
					(OS_FLAG_GRP*) &EventFlags,  //事件标志组
					(OS_FLAGS) 0x01, //事件位
					(OS_TICK) 0,    //超时时间
					(OS_OPT) OS_OPT_PEND_FLAG_SET_ALL + OS_OPT_PEND_FLAG_CONSUME, //等待置位并清除
					(CPU_TS*) 0,    //时间戳
					(OS_ERR*) &err); //错误码
			_USART6_CTL = 0; //串口6接收使能
			//等待数据接收,超时50ms
			OSFlagPend(
					(OS_FLAG_GRP*) &EventFlags,  //事件标志组
					(OS_FLAGS) 0x02, //事件位
					(OS_TICK) 10,    //超时时间
					(OS_OPT) OS_OPT_PEND_FLAG_SET_ALL + OS_OPT_PEND_FLAG_CONSUME, //等待置位并清除
					(CPU_TS*) 0,    //时间戳
					(OS_ERR*) &err); //错误码
			if (err != OS_ERR_TIMEOUT) //正确接收到数据
			{
				app_frame_anasys(USART6_Fram.RxBuf, USART6_Fram.AccessLen);
				communicationStatu = 1;
			}
			else //超时未收到信号量信息
			{
				communicationStatu = 0;
			}
			index = (pbERROR << 3) | (pbStatu << 2) | (communicationStatu << 1)
					| (motorStatu << 0);
			//查状态表
			TCP_Params.statuCode[i] = bat_statuBuf[index];
			//增加判断============Begin============
			if (TCP_Params.statuCode[i] == 11 || TCP_Params.statuCode[i] == 1) //有电池，锁弹出异常，通信正常
			{
				if (MyFlashParams.IgnoreLock[i] == 1) //设置了忽略锁状态
				{
					TCP_Params.statuCode[i] =
							(TCP_Params.statuCode[i] == 11) ? 10 : 0;
				}
			}
			//增加判断============End============
			setBATInstruction(i, TCP_Params.statuCode[i]);  //配置卡口状态
			//清除当前状态数组的值
			memset(PowerbankSTA.powerBankBuf[i], '\0', 18);
			if (communicationStatu) //如果正常有通信
			{
				if (pbERROR) //充电宝上报错误信息
				{
					snprintf(PowerbankSTA.powerBankBuf[i], 18, "%d_%d_%02X_%s",
							i, TCP_Params.statuCode[i], PowerbankSTA.ERROR,
							PowerbankSTA.BatID);
				}
				else //正常通信状态
				{
					snprintf(PowerbankSTA.powerBankBuf[i], 18, "%d_%d_%d_%s", i,
							TCP_Params.statuCode[i], PowerbankSTA.VOL,
							PowerbankSTA.BatID);
				}
			}
			else //未接收到充电宝发来的数据
			{
				snprintf(PowerbankSTA.powerBankBuf[i], 18, "%d_%d", i,
						TCP_Params.statuCode[i]);
			}
			//===========================================================================
			if (TCP_Params.statuCode[i] != TCP_Params.currentStatuCode[i]) //卡口状态发生变化了
			{
				TCP_Params.currentStatuCode[i] = TCP_Params.statuCode[i];
				//通知上报卡口任务
				OSTaskQPost((OS_TCB*) &ReportTaskTCB,	//待发送消息的任务控制块
						(void*) PowerbankSTA.powerBankBuf[i],      //待发送的数据
						(OS_MSG_SIZE) 10,        //发送的数据大小(字节数)
						(OS_OPT) OS_OPT_POST_FIFO,  //添加到消息队列尾
						(OS_ERR*) &err);     //返回的错误码
			}
		}
		//推送扫描完成
		OSFlagPost((OS_FLAG_GRP*) &EventFlags, //对应的事件标志组
				(OS_FLAGS) 0x04, //事件位
				(OS_OPT) OS_OPT_POST_FLAG_SET, //选择置位
				(OS_ERR*) &err); //错误码
		//每200ms检测一次卡口状态
		OSTimeDlyHMSM(0, 0, 0, 200, OS_OPT_TIME_PERIODIC, &err);
	}
}

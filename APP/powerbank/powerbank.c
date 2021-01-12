/*
 * powerbank.c
 *
 *  Created on: 2021年1月6日
 *      Author: HHS007
 */
#include "powerbank.h"

//当前卡口
vu8 curPort = 0xFF;
struct USART6_Fram_TypeDef USART6_Fram =
{ 0 };
struct Powerbank_Params_TypeDef PowerbankSTA =
{ 0 };

/**卡口状态*/
const unsigned char bat_statuBuf[16] =
{ 1, 0, 9, 8, 3, 2, 11, 10, 0xFF, 0xFF, 5, 4, 0xFF, 0xFF, 7, 6 };

void USART6_Init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	USART_DeInit(USART6);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE); //使能USART6时钟
	//=====================================================================================================
	DMA_USART_Tx_Init(USART6, RCC_AHB1Periph_DMA2, DMA2_Stream6_IRQn, 2, 0,
	DMA2_Stream6, DMA_Channel_5, (uint32_t) (&USART6->DR),
			(uint32_t) USART6_Fram.TxBuf, USART6_TX_BUF_LEN, DMA_Priority_High);

	DMA_USART_Rx_Init(USART6, RCC_AHB1Periph_DMA2, DMA2_Stream1, DMA_Channel_5,
			(uint32_t) (&USART6->DR), (uint32_t) USART6_Fram.RxBuf,
			USART6_RX_BUF_LEN, DMA_Priority_VeryHigh);
	//=====================================================================================================
	USART_InitStructure.USART_BaudRate = bound; //波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No; //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None; //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART6, &USART_InitStructure); //初始化串口

	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);  //根据指定的参数初始化VIC寄存器

	//中断配置
	USART_ITConfig(USART6, USART_IT_TC, DISABLE);
	USART_ITConfig(USART6, USART_IT_RXNE, DISABLE);
	USART_ITConfig(USART6, USART_IT_TXE, DISABLE);
	USART_ITConfig(USART6, USART_IT_IDLE, ENABLE);
	//启动串口
	USART_Cmd(USART6, ENABLE);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); //使能GPIOC时钟
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
 * 选择与哪个口的充电宝进行通信
 * @port 数字1-12
 */
void communicateWithPort(u8 port)
{
	switch (port)
	{
	case 1:
		_A1_1 = 0;
		_A0_1 = 0;
		break;
	case 2:
		_A1_1 = 0;
		_A0_1 = 1;
		break;
	case 3:
		_A1_1 = 1;
		_A0_1 = 0;
		break;
	case 4:
		_A1_1 = 1;
		_A0_1 = 1;
		_A1_2 = 0;
		_A0_2 = 0;
		break;
	case 5:
		_A1_1 = 1;
		_A0_1 = 1;
		_A1_2 = 0;
		_A0_2 = 1;
		break;
	case 6:
		_A1_1 = 1;
		_A0_1 = 1;
		_A1_2 = 1;
		_A0_2 = 0;
		break;
	case 7:
		_A1_1 = 1;
		_A0_1 = 1;
		_A1_2 = 1;
		_A0_2 = 1;
		_A1_3 = 0;
		_A0_3 = 0;
		break;
	case 8:
		_A1_1 = 1;
		_A0_1 = 1;
		_A1_2 = 1;
		_A0_2 = 1;
		_A1_3 = 0;
		_A0_3 = 1;
		break;
	case 9:
		_A1_1 = 1;
		_A0_1 = 1;
		_A1_2 = 1;
		_A0_2 = 1;
		_A1_3 = 1;
		_A0_3 = 0;
		break;
	case 10:
		_A1_1 = 1;
		_A0_1 = 1;
		_A1_2 = 1;
		_A0_2 = 1;
		_A1_3 = 1;
		_A0_3 = 1;
		_A1_4 = 0;
		_A0_4 = 0;
		break;
	case 11:
		_A1_1 = 1;
		_A0_1 = 1;
		_A1_2 = 1;
		_A0_2 = 1;
		_A1_3 = 1;
		_A0_3 = 1;
		_A1_4 = 0;
		_A0_4 = 1;
		break;
	case 12:
		_A1_1 = 1;
		_A0_1 = 1;
		_A1_2 = 1;
		_A0_2 = 1;
		_A1_3 = 1;
		_A0_3 = 1;
		_A1_4 = 1;
		_A0_4 = 0;
		break;
	default:
		break;
	}
}

void Powerbank_Init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); //使能GPIOC时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_14
			| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	Charge_Init();
	USART6_Init(bound);
}

/**
 * CRC校验函数
 * @pbuf 待校验的数组
 * @num 待校验的个数
 * @return 校验后的crc值
 */
unsigned int get_crc(unsigned char pbuf[], unsigned char num)
{
	unsigned char i, j;
	unsigned int wcrc = 0x0000;
	for (i = 0; i < num; i++)
	{
		wcrc ^= (pbuf[i] << 8);
		for (j = 0; j < 8; j++)
		{
			if (wcrc & 0x8000)
			{
				wcrc <<= 1;
				wcrc ^= 0x1021;
			}
			else
			{
				wcrc <<= 1;
			}
		}
	}
	return wcrc;
}
/**
 * CRC16校验函数
 * 可变参，@num为进行校验的数量
 */
unsigned int get_crc_2(int num, ...)
{
	va_list ap;  //声明参数指针
	unsigned int wcrc = 0x0000;
	unsigned char j = 0;
	va_start(ap, num); //设置参数指针
	while (num)
	{
		wcrc ^= (va_arg(ap, int) << 8);
		for (j = 0; j < 8; j++)
		{
			if (wcrc & 0x8000)
			{
				wcrc <<= 1;
				wcrc ^= 0x1021;
			}
			else
			{
				wcrc <<= 1;
			}
		}
		num--;
	}
	va_end(ap); //释放指针资源
	return wcrc;
}
/**
 * 帧协议解析
 * @data 待解析的数据
 * @size 数据长度
 */
void app_frame_anasys(u8 *data, u16 size)
{
	u8 Hinx = 0; //数据头位置
	u8 len = 0;  //帧长度
	u8 cmd = 0;  //控制字
	u16 crc = 0; //crc校验值
	u8 *crcBuf;  //待校验的crc数据缓冲区
	u8 *temBuf;  //完整帧数据缓冲区
	//寻找数据头
	for (int i = 0; i < size - 3; i++)
	{
		if (data[i] == BAT_Up_Header)
		{
			Hinx = i; //数据头的位置
			len = data[Hinx + 1]; //本帧数据的长度
			cmd = data[Hinx + 2]; //本帧数据携带的控制字
			break;
		}
	}
	//判断数据完整性
	if (len <= (size - Hinx))
	{
		//包含了一帧完整的数据
		if (data[Hinx + len - 1] == BAT_Up_End)
		{
			crcBuf = mymalloc(len - 4); //为crc申请空间
			temBuf = mymalloc(len);     //为帧申请空间
			for (int i = 0; i < len; i++, Hinx++)
			{
				temBuf[i] = data[Hinx];
				if (i < len - 4)
				{
					crcBuf[i] = data[Hinx + 1];
				}
			}
			crc = get_crc(crcBuf, len - 4);
			myfree(crcBuf); //释放crc缓冲区
			if (crc == ((data[len - 3] << 8) | data[len - 2])) //crc校验成功
			{
				app_cmd_anasys(temBuf, cmd);
			}
			myfree(temBuf); //释放帧缓冲区
		}
	}
}
/**
 * 帧数据解析
 * @data 完整的数据
 * @cmd  控制字
 */
void app_cmd_anasys(u8 *data, u8 cmd)
{
	switch (cmd)
	{
	case BAT_Up_Admin:
		PowerbankSTA.VOL = data[11];
		for (int inx = 0; inx < 8; inx++)
		{
			PowerbankSTA.BatID[inx] = data[inx + 3];
		}
		PowerbankSTA.BatID[8] = '\0';
		break;
	case BAT_ComTest:

		break;
	case BAT_Up_ReadID:
		for (int inx = 0; inx < 8; inx++)
		{
			PowerbankSTA.BatID[inx] = data[inx + 3];
		}
		PowerbankSTA.BatID[8] = '\0';
		break;
	case BAT_SetID_SUCCESS:
		for (int inx = 0; inx < 8; inx++)
		{
			PowerbankSTA.BatID[inx] = data[inx + 3];
		}
		PowerbankSTA.BatID[8] = '\0';
		break;
	case BAT_Up_ReadVOL:
		PowerbankSTA.VOL = data[11];
		for (int inx = 0; inx < 8; inx++)
		{
			PowerbankSTA.BatID[inx] = data[inx + 3];
		}
		PowerbankSTA.BatID[8] = '\0';
		break;
	case BAT_Up_ReadCUR:
		PowerbankSTA.CUR = (data[11] << 8) | data[12];
		for (int inx = 0; inx < 8; inx++)
		{
			PowerbankSTA.BatID[inx] = data[inx + 3];
		}
		PowerbankSTA.BatID[8] = '\0';
		break;
	case BAT_Up_ReadERROR:
		PowerbankSTA.ERROR = data[11];
		for (int inx = 0; inx < 8; inx++)
		{
			PowerbankSTA.BatID[inx] = data[inx + 3];
		}
		PowerbankSTA.BatID[8] = '\0';
		break;
	default:
		break;
	}
}

/**
 * 根据检测的状态设置
 * @port 0-11
 * @sta 状态
 */
void setBATInstruction(u8 port, u8 sta)
{
	if (sta >= 0 && sta <= 11)
	{
		if (sta == 10) //卡口正常状态
		{
			ledON(port + 1);
			HC595_STATUS.slowBLINK[port] = 0; //慢闪
			HC595_STATUS.fastBLINK[port] = 0;
			if (PowerbankSTA.VOL < 95) //电量不满95%充电
			{
				PowerbankSTA.Charging[port] = 1; //表明在充电中
				controlPowerBankCharge(port + 1, 1); //充电
			}
			else
			{
				PowerbankSTA.Charging[port] = 0; //表明未在充电中
				controlPowerBankCharge(port + 1, 0); //不充电
			}
		}
		else if (sta == 0) //正常无充电宝状态
		{
			PowerbankSTA.Charging[port] = 0;
			controlPowerBankCharge(port + 1, 0); //不充电
			ledOFF(port + 1);
			HC595_STATUS.slowBLINK[port] = 0; //慢闪
			HC595_STATUS.fastBLINK[port] = 0;
		}
		else //有异常
		{
			PowerbankSTA.Charging[port] = 0;
			controlPowerBankCharge(port + 1, 0); //不充电
			HC595_STATUS.slowBLINK[port] = 0; //慢闪
			HC595_STATUS.fastBLINK[port] = 1; //快闪
		}
	}
}
/**
 * 检查充电宝状态
 * @i 0-11
 */
u8 checkPowerbankStatus(u8 i)
{
	vu8 pbStatu = 0;
	vu8 motorStatu = 0;
	vu8 communicationStatu = 0;
	vu8 index = 0;
	u8 pbERROR = 0;
	u16 len = 0;
	vu8 portSTA = 0;
	OS_ERR err;
	len = fillDataToTxBuf(BAT_Down_Admin, 0, NULL);
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
	OSFlagPend((OS_FLAG_GRP*) &EventFlags,  //事件标志组
			(OS_FLAGS) FLAG_USART6_TxED, //事件位
			(OS_TICK) 0,    //超时时间
			(OS_OPT) OS_OPT_PEND_FLAG_SET_ALL + OS_OPT_PEND_FLAG_CONSUME, //等待置位并清除
			(CPU_TS*) 0,    //时间戳
			(OS_ERR*) &err); //错误码
	_USART6_CTL = 0; //串口6接收使能
	//等待数据接收,超时50ms
	OSFlagPend((OS_FLAG_GRP*) &EventFlags,  //事件标志组
			(OS_FLAGS) FLAG_USART6_RxED, //事件位
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
	portSTA = bat_statuBuf[index];
	//增加判断============Begin============
	if (portSTA == 11 || portSTA == 1) //有电池，锁弹出异常，通信正常
	{
		if (MyFlashParams.IgnoreLock[i] == 1) //设置了忽略锁状态
		{
			portSTA = (portSTA == 11) ? 10 : 0;
		}
	}
	//增加判断============End============
	setBATInstruction(i, portSTA);  //配置卡口状态
	//清除当前状态数组的值
	memset(PowerbankSTA.powerBankBuf[i], '\0', 18);
	if (communicationStatu) //如果正常有通信
	{
		if (pbERROR) //充电宝上报错误信息
		{
			snprintf(PowerbankSTA.powerBankBuf[i], 18, "%d_%d_%02X_%s", i,
					portSTA, PowerbankSTA.ERROR, PowerbankSTA.BatID);
		}
		else //正常通信状态
		{
			snprintf(PowerbankSTA.powerBankBuf[i], 18, "%d_%d_%d_%s", i,
					portSTA, PowerbankSTA.VOL, PowerbankSTA.BatID);
		}
	}
	else //未接收到充电宝发来的数据
	{
		snprintf(PowerbankSTA.powerBankBuf[i], 18, "%d_%d", i, portSTA);
	}
	//===========================================================================
	return portSTA;
}

/**
 * 控制充电宝
 * @cmd 控制字
 * @allowSetID 是否是设置ID指令
 * @id  待设置的ID
 * @return 指令的长度
 */
u16 fillDataToTxBuf(u8 cmd, u8 allowSetID, u8 *id)
{
	u16 crc;
	USART6_Fram.TxBuf[0] = BAT_Down_Header;
	if (allowSetID)
	{
		u8 *crcBuf = mymalloc(0x0A);
		USART6_Fram.TxBuf[1] = 0x0E;
		USART6_Fram.TxBuf[2] = BAT_Down_SetID;
		crcBuf[0] = 0x0E;
		crcBuf[1] = BAT_Down_SetID;
		for (int i = 0; i < 8; i++)
		{
			USART6_Fram.TxBuf[i + 3] = id[i];
			crcBuf[i + 2] = id[i];
		}
		crc = get_crc(crcBuf, 0x0A);
		USART6_Fram.TxBuf[11] = (crc >> 8) & 0xFF;
		USART6_Fram.TxBuf[12] = crc & 0xFF;
		USART6_Fram.TxBuf[13] = BAT_Down_End;
		myfree(crcBuf);
		return (0x0E);
	}
	//其他
	USART6_Fram.TxBuf[1] = 0x06;
	USART6_Fram.TxBuf[2] = cmd;
	crc = get_crc_2(2, USART6_Fram.TxBuf[1], USART6_Fram.TxBuf[2]);
	USART6_Fram.TxBuf[3] = crc >> 8 & 0xFF;
	USART6_Fram.TxBuf[4] = crc & 0xFF;
	USART6_Fram.TxBuf[5] = BAT_Down_End;
	return (6);
}

/**
 * 串口6通过DMA发送数据
 * @data 待发送的数据
 * @return 无
 */
void USART6_DMA_Send(u16 size)
{
	//等待空闲
	while (USART6_Fram.DMA_Tx_Busy)
		;
	USART6_Fram.DMA_Tx_Busy = 1;
	//设置传输数据长度
	DMA_SetCurrDataCounter(DMA2_Stream6, size);
	//打开DMA,开始发送
	DMA_Cmd(DMA2_Stream6, ENABLE);
}

void DMA2_Stream6_IRQHandler(void)
{
#if SYSTEM_SUPPORT_OS
	OSIntEnter();
#endif
	if (DMA_GetITStatus(DMA2_Stream6, DMA_IT_TCIF6) != RESET)
	{
		//清除标志位
		DMA_ClearFlag(DMA2_Stream6, DMA_FLAG_TCIF6);
		//关闭DMA
		DMA_Cmd(DMA2_Stream6, DISABLE);
		//打开发送完成中断,发送最后两个字节
		USART_ITConfig(USART6, USART_IT_TC, ENABLE);
	}
#if SYSTEM_SUPPORT_OS
	OSIntExit(); //退出中断
#endif
}

void USART6_IRQHandler(void)                	//串口1中断服务程序
{
#if SYSTEM_SUPPORT_OS  //使用UCOS操作系统
	OS_ERR err;
	OSIntEnter();
#endif
	if (USART_GetITStatus(USART6, USART_IT_TC) != RESET)
	{
		//关闭发送完成中断
		USART_ITConfig(USART6, USART_IT_TC, DISABLE);
		//发送完成
		USART6_Fram.DMA_Tx_Busy = 0;
#if SYSTEM_SUPPORT_OS
		//推送发送完成
		OSFlagPost((OS_FLAG_GRP*) &EventFlags, //对应的事件标志组
				(OS_FLAGS) FLAG_USART6_TxED, //事件位
				(OS_OPT) OS_OPT_POST_FLAG_SET, //选择置位
				(OS_ERR*) &err); //错误码
#endif
	}
	if (USART_GetITStatus(USART6, USART_IT_IDLE) != RESET)
	{
		USART6->SR; //先读SR，再读DR
		USART6->DR;
		//关闭DMA
		DMA_Cmd(DMA2_Stream1, DISABLE);
		//清除标志位
		DMA_ClearFlag(DMA2_Stream1, DMA_FLAG_TCIF1);
		//获得接收帧帧长
		USART6_Fram.AccessLen = USART6_RX_BUF_LEN
				- DMA_GetCurrDataCounter(DMA2_Stream1);
		//这里可以通知任务来处理数据
#if SYSTEM_SUPPORT_OS
		//推送接收完成
		OSFlagPost((OS_FLAG_GRP*) &EventFlags, //对应的事件标志组
				(OS_FLAGS) FLAG_USART6_RxED, //事件位
				(OS_OPT) OS_OPT_POST_FLAG_SET, //选择置位
				(OS_ERR*) &err); //错误码
#endif
		//设置传输数据长度
		DMA_SetCurrDataCounter(DMA2_Stream1, USART6_RX_BUF_LEN);
		//打开DMA
		DMA_Cmd(DMA2_Stream1, ENABLE);
	}
#if SYSTEM_SUPPORT_OS
	OSIntExit(); //退出中断
#endif
}

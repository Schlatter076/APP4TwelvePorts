/*
 * powerbank.c
 *
 *  Created on: 2021年1月6日
 *      Author: HHS007
 */
#include "powerbank.h"

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

void app_cmd_anasys(u8 *data, u16 size)
{
	u8 inx = 0;
	u8 len = 0;
	u8 cmd = 0;
	u16 crc = 0;
	u8 *crcBuf;
	for(int i = 0; i < size - 3; i++)
	{
		if(data[i] == BAT_Up_Header)
		{
			inx = i;
			len = data[inx + 1];
			cmd = data[inx + 2];
			break;
		}
	}
	crcBuf = mymalloc(len - 4);
	for(int i = 0; i < len - 4; i++, inx++)
	{
		crcBuf[i] = data[inx + 1];
	}
	crc = get_crc(crcBuf, len - 4);
	myfree(crcBuf);
	if(len -2 < size)
	{
		if(crc == ((data[len - 3] << 8) | data[len -2])) //crc校验成功
		{

		}
	}
}

/**
 * 串口6通过DMA发送数据
 * @data 待发送的数据
 * @size 待发送数据的长度
 * @return 无
 */
void USART6_DMA_Send(u8 *data, u16 size)
{
	//等待空闲
	while (USART6_Fram.DMA_Tx_Busy)
		;
	USART6_Fram.DMA_Tx_Busy = 1;
	//复制数据
	memcpy(USART6_Fram.TxBuf, data, size);
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
	OSIntEnter();
#endif
	if (USART_GetITStatus(USART6, USART_IT_TC) != RESET)
	{
		//关闭发送完成中断
		USART_ITConfig(USART6, USART_IT_TC, DISABLE);
		//发送完成
		USART6_Fram.DMA_Tx_Busy = 0;
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

		//设置传输数据长度
		DMA_SetCurrDataCounter(DMA2_Stream1, USART6_RX_BUF_LEN);
		//打开DMA
		DMA_Cmd(DMA2_Stream1, ENABLE);
	}
#if SYSTEM_SUPPORT_OS
	OSIntExit(); //退出中断
#endif
}


/*
 * F4G.c
 *
 *  Created on: 2021年1月8日
 *      Author: HHS007
 */
#include "F4G.h"

void USART3_Init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	USART_DeInit(USART3);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	//=====================================================================================================
	DMA_USART_Tx_Init(USART3, RCC_AHB1Periph_DMA1, DMA1_Stream3_IRQn, 1, 1,
	DMA1_Stream3, DMA_Channel_4, (uint32_t) (&USART3->DR),
			(uint32_t) F4G_Fram.TxBuf, BASE64_BUF_LEN, DMA_Priority_High);

	DMA_USART_Rx_Init(USART3, RCC_AHB1Periph_DMA1, DMA1_Stream1, DMA_Channel_4,
			(uint32_t) (&USART3->DR), (uint32_t) F4G_Fram.RxBuf,
			TCP_MAX_LEN, DMA_Priority_VeryHigh);
	//=====================================================================================================
	USART_InitStructure.USART_BaudRate = bound; //波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No; //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None; //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART3, &USART_InitStructure); //初始化串口

	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);  //根据指定的参数初始化VIC寄存器

	//中断配置
	USART_ITConfig(USART3, USART_IT_TC, DISABLE);
	USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
	USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
	//启动串口
	USART_Cmd(USART3, ENABLE);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //使能GPIO时钟
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void F4G_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	USART3_Init(bound);
}

void DMA1_Stream3_IRQHandler(void)
{
#if SYSTEM_SUPPORT_OS
	OSIntEnter();
#endif
	if (DMA_GetITStatus(DMA1_Stream3, DMA_IT_TCIF3) != RESET)
	{
		//清除标志位
		DMA_ClearFlag(DMA1_Stream3, DMA_FLAG_TCIF3);
		//关闭DMA
		DMA_Cmd(DMA1_Stream3, DISABLE);
		//打开发送完成中断,发送最后两个字节
		USART_ITConfig(USART3, USART_IT_TC, ENABLE);
	}
#if SYSTEM_SUPPORT_OS
	OSIntExit(); //退出中断
#endif
}

void USART3_IRQHandler(void)
{
#if SYSTEM_SUPPORT_OS  //使用UCOS操作系统
	OS_ERR err;
	OSIntEnter();
#endif
	if (USART_GetITStatus(USART3, USART_IT_TC) != RESET)
	{
		//关闭发送完成中断
		USART_ITConfig(USART3, USART_IT_TC, DISABLE);
		//发送完成
		F4G_Fram.DMA_Tx_Busy = 0;
#if SYSTEM_SUPPORT_OS
		//推送发送完成
		OSFlagPost((OS_FLAG_GRP*) &EventFlags, //对应的事件标志组
				(OS_FLAGS) 0x20, //事件位
				(OS_OPT) OS_OPT_POST_FLAG_SET, //选择置位
				(OS_ERR*) &err); //错误码
#endif
	}
	if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
	{
		USART3->SR; //先读SR，再读DR
		USART3->DR;
		//关闭DMA
		DMA_Cmd(DMA1_Stream1, DISABLE);
		//清除标志位
		DMA_ClearFlag(DMA1_Stream1, DMA_FLAG_TCIF1);
		//获得接收帧帧长
		F4G_Fram.AccessLen = TCP_MAX_LEN
				- DMA_GetCurrDataCounter(DMA1_Stream1);
		//这里可以通知任务来处理数据
#if SYSTEM_SUPPORT_OS
		//推送接收完成
		OSFlagPost((OS_FLAG_GRP*) &EventFlags, //对应的事件标志组
				(OS_FLAGS) 0x40, //事件位
				(OS_OPT) OS_OPT_POST_FLAG_SET, //选择置位
				(OS_ERR*) &err); //错误码
#endif
		//设置传输数据长度
		DMA_SetCurrDataCounter(DMA1_Stream1, TCP_MAX_LEN);
		//打开DMA
		DMA_Cmd(DMA1_Stream1, ENABLE);
	}
#if SYSTEM_SUPPORT_OS
	OSIntExit();    	//退出中断
#endif
}


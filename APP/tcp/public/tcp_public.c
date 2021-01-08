/*
 * tcp_public.c
 *
 *  Created on: 2021年1月5日
 *      Author: HHS007
 */
#include "tcp_public.h"

struct STRUCT_USART_Fram F4G_Fram =
{ 0 };
struct STRUCT_USART_Fram WIFI_Fram =
{ 0 };
struct STRUCT_USART_Fram USART1_Fram =
{ 0 };
struct STRUCT_USART_Params TCP_Params =
{ 0 };
struct RegisterFram RegisterParams =
{ 0 };

/**
 * 拆分服务器下发的指令
 */
void AnalyzeServerParams(void)
{
	u8 inx = 0;
	char *tem = (char *) MyFlashParams.ServerParams;
	char *buf;
	DEBUG("RegisterParams=%s\r\n", tem);
	//获取秘钥
	while (*tem != '-')
	{
		RegisterParams.key[inx++] = *tem;
		tem++;
	}
	RegisterParams.key[inx] = '\0'; //添加结束符
	tem++; //跳过'-'字符
	inx = 0;
	//获取ip
	while (*tem != ':')
	{
		RegisterParams.ip[inx++] = *tem;
		tem++;
	}
	RegisterParams.ip[inx] = '\0';
	tem++; //跳过':'
	inx = 0;
	while (*tem != '-')
	{
		RegisterParams.port[inx++] = *tem;
		tem++;
	}
	RegisterParams.port[inx] = '\0';
	tem++;
	inx = 0;
	buf = mymalloc(3);
	memset(buf, '\0', 3);
	while (*tem != '-')
	{
		buf[inx++] = *tem;
		tem++;
	}
	RegisterParams.needConfirmParams = atoi(buf);
	tem++;
	inx = 0;
	memset(buf, '\0', 3);
	while (*tem != '-')
	{
		buf[inx++] = *tem;
		tem++;
	}
	RegisterParams.motor_TCtime = atoi(buf);
	tem++;
	inx = 0;
	memset(buf, '\0', 3);
	while (*tem != '-')
	{
		buf[inx++] = *tem;
		tem++;
	}
	RegisterParams.motor_HTtime = atoi(buf);
	myfree(buf);
	DEBUG("key=%s,ip=%s,port=%s,confirm=%d,TCtime=%d,HTtime=%d\r\n",
			RegisterParams.key, RegisterParams.ip, RegisterParams.port,
			RegisterParams.needConfirmParams, RegisterParams.motor_TCtime,
			RegisterParams.motor_HTtime);
}
/**
 * 串口格式化输出数据
 * @net 选择端口
 * @fmt 格式化参数
 * @return 无
 */
void _USART_Printf(ENUM_Internet_TypeDef net, const char *fmt, ...)
{
	va_list ap;
	struct STRUCT_USART_Fram *fram;
	DMA_Stream_TypeDef* DMAy_Streamx;
	if (net == In4G)
	{
		fram = &F4G_Fram;
		DMAy_Streamx = DMA1_Stream3;
	}
	else if (net == InWifi)
	{
		fram = &WIFI_Fram;
		DMAy_Streamx = DMA1_Stream6;
	}
	else
	{
		fram = &USART1_Fram;
		DMAy_Streamx = DMA2_Stream7;
	}
	//等待空闲,防止数据不同步
	while (fram->DMA_Tx_Busy)
		;
	fram->DMA_Tx_Busy = 1;
	va_start(ap, fmt);
	vsprintf((char *) fram->TxBuf, fmt, ap);
	va_end(ap);
	//设置传输数据长度
	DMA_SetCurrDataCounter(DMAy_Streamx, strlen((const char *) fram->TxBuf));
	//打开DMA,开始发送
	DMA_Cmd(DMAy_Streamx, ENABLE);
}
/**
 * 串口1调试打印
 * DMA方式发送
 */
void DEBUG(const char *fmt, ...)
{
	va_list ap;
	//等待空闲,防止数据不同步
	while (USART1_Fram.DMA_Tx_Busy)
		;
	USART1_Fram.DMA_Tx_Busy = 1;
	va_start(ap, fmt);
	vsprintf((char *) USART1_Fram.TxBuf, fmt, ap);
	va_end(ap);
	//设置传输数据长度
	DMA_SetCurrDataCounter(DMA2_Stream7,
			strlen((const char *) USART1_Fram.TxBuf));
	//打开DMA,开始发送
	DMA_Cmd(DMA2_Stream7, ENABLE);
}

/**
 * 对模块发送AT指令
 * @cmd：待发送的指令
 * @ack1，@ack2：期待的响应，为NULL表不需响应，两者为或逻辑关系
 * @time：等待响应的时间(时间片长度)
 * @return 1：发送成功 0：失败
 */
bool Send_AT_Cmd(ENUM_Internet_TypeDef internet, char *cmd, char *ack1,
		char *ack2, u32 time)
{
	struct STRUCT_USART_Fram *USART_Fram;
	OS_FLAGS flag;
	OS_ERR err;

	if (internet == In4G)
	{
		USART_Fram = &F4G_Fram;
		flag = FLAG_F4G_AT;
	}
	else if (internet == InWifi)
	{
		USART_Fram = &WIFI_Fram;
		flag = FLAG_WIFI_AT;
	}
	_USART_Printf(internet, "%s\r\n", cmd); //发送指令
	if (ack1 == 0 && ack2 == 0)	 //不需要接收数据
	{
		return true;
	}
	OSFlagPend((OS_FLAG_GRP*) &EventFlags,  //事件标志组
			(OS_FLAGS) flag, //事件位
			(OS_TICK) time,    //超时时间
			(OS_OPT) OS_OPT_PEND_FLAG_SET_ALL + OS_OPT_PEND_FLAG_CONSUME, //等待置位并清除
			(CPU_TS*) 0,    //时间戳
			(OS_ERR*) &err); //错误码

	//调试打印
	DEBUG("%s\r\n", USART_Fram->RxBuf);
	if (err == OS_ERR_NONE)
	{
		if (ack1 != 0 && ack2 != 0)
		{
			return (( bool ) strstr((const char *) USART_Fram->RxBuf, ack1)
					|| ( bool ) strstr((const char *) USART_Fram->RxBuf, ack2));
		}
		else if (ack1 != 0)
		{
			return (( bool ) strstr((const char *) USART_Fram->RxBuf, ack1));
		}
		else
		{
			return (( bool ) strstr((const char *) USART_Fram->RxBuf, ack2));
		}
	}
	return false;
}

bool AT_Test(ENUM_Internet_TypeDef internet)
{
	u8 count = 0;
	char *module = mymalloc(10);
	if (internet == In4G)
	{
		sprintf(module, "%s", "4G module");
	}
	else
	{
		sprintf(module, "%s", "WIFI module");
	}
	while (count++ < 8)
	{
		Send_AT_Cmd(internet, "AT", "OK", NULL, 100);
	}
	if (Send_AT_Cmd(internet, "AT", "OK", NULL, 100))
	{
		DEBUG("test %s success!\r\n", module);
		myfree(module);
		return 1;
	}
	DEBUG("test %s fail!\r\n", module);
	myfree(module);
	return 0;
}

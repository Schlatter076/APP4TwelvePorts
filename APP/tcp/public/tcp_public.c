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
	_USART_Printf(InUsart1, "RegisterParams=%s\r\n", tem);
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
	_USART_Printf(InUsart1,
			"key=%s,ip=%s,port=%s,confirm=%d,TCtime=%d,HTtime=%d\r\n",
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
 * 对模块发送AT指令
 * @cmd：待发送的指令
 * @ack1，@ack2：期待的响应，为NULL表不需响应，两者为或逻辑关系
 * @time：等待响应的时间
 * @return 1：发送成功 0：失败
 */
bool Send_AT_Cmd(ENUM_Internet_TypeDef internet, char *cmd, char *ack1,
		char *ack2, u32 time)
{
	struct STRUCT_USART_Fram *USART_Fram;

	if (internet == In4G)
	{
		USART_Fram = &F4G_Fram;
	}
	else
	{
		USART_Fram = &WIFI_Fram;
	}
	USART_Fram->InfBit.Length = 0;	//从新开始接收新的数据包
	_USART_printf(internet, "%s\r\n", cmd);
	if (ack1 == 0 && ack2 == 0)	 //不需要接收数据
	{
		return true;
	}
	delay_ms(time);	  //延时time时间

	USART_Fram->Data[USART_Fram->InfBit.Length] = '\0';

	printf("%s", USART_Fram->Data);

	if (ack1 != 0 && ack2 != 0)
	{
		//USART_Fram->InfAll = 0;
		return (( bool ) strstr((const char *) USART_Fram->Data, ack1)
				|| ( bool ) strstr((const char *) USART_Fram->Data, ack2));
	}
	else if (ack1 != 0)
	{
		//USART_Fram->InfAll = 0;
		return (( bool ) strstr((const char *) USART_Fram->Data, ack1));
	}
	else
	{
		//USART_Fram->InfAll = 0;
		return (( bool ) strstr((const char *) USART_Fram->Data, ack2));
	}
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
		Send_AT_Cmd(internet, "AT", "OK", NULL, 500);
	}
	if (Send_AT_Cmd(internet, "AT", "OK", NULL, 500))
	{
		printf("test %s success!\r\n", module);
		myfree(module);
		return 1;
	}
	printf("test %s fail!\r\n", module);
	myfree(module);
	return 0;
}


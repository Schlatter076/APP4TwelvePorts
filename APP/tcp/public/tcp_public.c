/*
 * tcp_public.c
 *
 *  Created on: 2021年1月5日
 *      Author: HHS007
 */
#include "tcp_public.h"

struct STRUCT_USART_Fram F4G_Fram = {0};
struct STRUCT_USART_Fram WIFI_Fram = {0};
struct STRUCT_USART_Fram USART1_Fram = {0};
struct STRUCT_USART_Params TCP_Params = {0};
struct RegisterFram RegisterParams = {0};

/**
 * 拆分服务器下发的指令
 */
void AnalyzeServerParams(void)
{
	u8 inx = 0;
	char *tem = (char *)MyFlashParams.ServerParams;
	char *buf;
	printf("RegisterParams=%s\r\n", tem);
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
	printf("key=%s,ip=%s,port=%s,confirm=%d,TCtime=%d,HTtime=%d\r\n",
			RegisterParams.key, RegisterParams.ip, RegisterParams.port,
			RegisterParams.needConfirmParams, RegisterParams.motor_TCtime,
			RegisterParams.motor_HTtime);
}

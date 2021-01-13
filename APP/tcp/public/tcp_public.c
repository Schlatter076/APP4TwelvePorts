/*
 * tcp_public.c
 *
 *  Created on: 2021年1月5日
 *      Author: HHS007
 */
#include "tcp_public.h"

const char Heart[] = "";

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
	while (*tem)
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
 * @retry 重试次数
 * @return 1：发送成功 0：失败
 */
bool Send_AT_Cmd(ENUM_Internet_TypeDef internet, char *cmd, char *ack1,
		char *ack2, u32 time, u16 retry, FunctionalState printState)
{
	struct STRUCT_USART_Fram *USART_Fram = NULL;
	OS_FLAGS flag = 0;
	OS_ERR err;
	bool ret = false;
	u16 cnt = 0;

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
	while (ret == false && cnt++ < retry)
	{
		OSFlagPend((OS_FLAG_GRP*) &EventFlags,  //事件标志组
				(OS_FLAGS) flag, //事件位
				(OS_TICK) time,    //超时时间
				(OS_OPT) OS_OPT_PEND_FLAG_SET_ALL + OS_OPT_PEND_FLAG_CONSUME, //等待置位并清除
				(CPU_TS*) 0,    //时间戳
				(OS_ERR*) &err); //错误码

		//调试打印
		if (printState)
		{
			DEBUG("%s\r\n", USART_Fram->RxBuf);
		}

		if (err == OS_ERR_NONE)
		{
			if (ack1 != 0 && ack2 != 0)
			{
				if (strstr((const char *) USART_Fram->RxBuf, ack1)
						|| strstr((const char *) USART_Fram->RxBuf, ack2))
				{
					ret = true;
				}
			}
			else if (ack1 != 0)
			{
				if (strstr((const char *) USART_Fram->RxBuf, ack1))
				{
					ret = true;
				}
			}
			else if (ack2 != 0)
			{
				if (strstr((const char *) USART_Fram->RxBuf, ack2))
				{
					ret = true;
				}
			}
			else if (strstr((const char *) USART_Fram->RxBuf, "ERROR"))
			{
				ret = false;
			}
		}
	}
	return ret;
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
		Send_AT_Cmd(internet, "AT", "OK", NULL, 100, 2, DISABLE);
	}
	if (Send_AT_Cmd(internet, "AT", "OK", NULL, 100, 2, DISABLE))
	{
		DEBUG("test %s success!\r\n", module);
		myfree(module);
		return 1;
	}
	DEBUG("test %s fail!\r\n", module);
	myfree(module);
	return 0;
}
/**
 * 获取wifi名和密码
 * @datas UTF-8编码的字符串，包含长度和内容
 * 		      如："19_9_黑火石w1c. 28！_hhs888888"
 * 无返回
 */
void getWifiSsidAndPwd(char *datas)
{
	char *res = datas;
	u8 Inx = 0;
	char buf[4] =
	{ '\0', '\0', '\0', '\0' };
	u16 ssidLEN = 0;
	u16 pwdLEN = 0;
	while (*res != '_' && Inx < 3)
	{
		buf[Inx++] = *res++;
	}
	res++;
	ssidLEN = atoi(buf);
	DEBUG("ssid_len=%d\r\n", ssidLEN);
	Inx = 0;
	memset(buf, '\0', 4);
	while (*res != '_' && Inx < 3)
	{
		buf[Inx++] = *res++;
	}
	res++;
	pwdLEN = atoi(buf);
	DEBUG("pwd_len=%d\r\n", pwdLEN);
	memset(MyFlashParams.WifiSSID, '\0', 101);
	memset(MyFlashParams.WifiPWD, '\0', 101);
	Inx = 0;
	while (ssidLEN--)
	{
		MyFlashParams.WifiSSID[Inx++] = *res++;
	}
	res++;
	Inx = 0;
	while (pwdLEN--)
	{
		MyFlashParams.WifiPWD[Inx++] = *res++;
	}
	MyFlashParams.WifiFlag = WIFI_FLAG;
	MyFlash_Write(&MyFlashParams);
	DEBUG("SSID=%s,PWD=%s\r\n", MyFlashParams.WifiSSID, MyFlashParams.WifiPWD);
}

/**
 * 获取注册字符串
 * @strBuf 待获取的字符串
 * @len 字符串长度
 * @upCMD 上行控制字
 * @modulType 所使用的的模块类型
 * 无返回
 */
void getRegisterStr(char *strBuf, int len, ENUM_tcpUP_TypeDef upCMD,
		char moduleType)
{
	const char* template = "%s,%d,%s-%c-%c-%s-%s_%s-%s";
	snprintf(strBuf, len, template, MyFlashParams.DeviceID, upCMD,
			TCP_Params.ccid, TCP_Params.cops, moduleType, MyFlashParams.Version,
			"0", "0", "12");
}
/**
 * 获取不带参数的请求字符串
 * @strBuf
 * @len
 * @upCMD 上行控制字
 * 无返回
 */
void getRequestStrWithoutParam(char *strBuf, int len, ENUM_tcpUP_TypeDef upCMD)
{
	const char* template = "%s,%d";
	snprintf(strBuf, len, template, MyFlashParams.DeviceID, upCMD);
}
/**
 * 获取充电宝状态字符串
 * @strBuf
 * @len
 * @upCMD
 * @rssi 信号(当使用4G模块时)
 * @portNum 卡口数
 * @portSTAStr 状态字符串(可变参)
 * 无返回
 */
void getPowerbankSTAStr(char *strBuf, int len, ENUM_tcpUP_TypeDef upCMD,
		u8 rssi, int portNum, char *portSTAStr, ...)
{
	va_list ap;
	char *tem;
	va_start(ap, portSTAStr);
	snprintf(strBuf, len, "%s,%d,%s", MyFlashParams.DeviceID, upCMD,
			portSTAStr);
	for (int i = 1; i < portNum; i++)
	{
		tem = va_arg(ap, char *);
		strcat(strBuf, "-");
		strcat(strBuf, tem);
	}
	va_end(ap);
	char *temBuf = mymalloc(3);
	memset(temBuf, '\0', 3);
	snprintf(temBuf, 3, "%d", rssi);
	strcat(strBuf, "-");
	strcat(strBuf, temBuf);
	myfree(temBuf);
	strcat(strBuf, "-aaaaa");
}
/**
 * 获取充电宝状态字符串(不带信号)
 * @strBuf
 * @len
 * @upCMD
 * @portNum 卡口数
 * @portSTAStr 状态字符串(可变参)
 * 无返回
 */
void getPowerbankSTAStrWithoutRSSI(char *strBuf, int len,
		ENUM_tcpUP_TypeDef upCMD, int portNum, char *portSTAStr, ...)
{
	va_list ap;
	va_start(ap, portSTAStr);
	snprintf(strBuf, len, "%s,%d,%s", MyFlashParams.DeviceID, upCMD,
			portSTAStr);
	for (int i = 1; i < portNum; i++)
	{
		char *tem = va_arg(ap, char *);
		strcat(strBuf, "-");
		strcat(strBuf, tem);
	}
	va_end(ap);
	strcat(strBuf, "-aaaaa");
}
/**
 * 处理服务端指令
 * @intenet 4G 或 Wifi
 * @DownCMD 下行指令
 * @SData 携带的信息体
 * 无返回
 */
void ProcessServerCmd(ENUM_Internet_TypeDef internet,
		ENUM_tcpDOWN_TypeDef DownCMD, char *SData)
{
	switch (DownCMD)
	{
	case DOWN_RegiseterSucc:
		getRegisterParams(internet, SData);
		break;
	case DOWN_RecivedAllPortsSTA:
		if (internet == In4G)
		{
			F4G_Fram.firstStatuHeartNotSucc = 0;
		}
		else if (internet == InWifi)
		{
			WIFI_Fram.firstStatuHeartNotSucc = 0;
		}
		break;
	case DOWN_RecivedStatuHeart:

		break;
	case DOWN_SystemPopupPowerbank:
		systemPopup(internet, SData);
		break;
	case DOWN_RecivedForceHeart:

		break;
	case DOWN_RecivedSystemPopupSTA:

		break;
	case DOWN_RecivedPowerbankSTAChanged:

		break;
	case DOWN_VoiceBroadcast:

		break;
	case DOWN_ForceHeart:
		forceHeart(internet, UP_ForceHeart, ENABLE);
		break;
	case DOWN_DeviceReset:
		responseReset(internet);
		break;
	case DOWN_OrderPopupPowerbank:
		orderPopup(internet, SData);
		break;
	case DOWN_RecivedOrderPopupPowerbank:

		break;
	case DOWN_PopupAllPowerbanks:
		popUP_All();
		break;
	case DOWN_IgnoreLock:
		modifyLockSTA(SData);
		break;
	case DOWN_SetWifiSsidAndPwd:
		setWifiSsidAndPwd(internet, SData);
		break;
	case DOWN_SetID:
		memcpy(MyFlashParams.DeviceID, SData, 8);
		MyFlashParams.DeviceID[8] = '\0';
		MyFlash_Write(&MyFlashParams);
		NVIC_SystemReset();
		break;
	case DOWN_RemoteCtrMotor:
		setMotorRun(SData);
	default:
		DEBUG("cmd\"%d\" is not support\r\n", DownCMD);
		break;
	}
}
/**
 * 系统弹出充电宝
 * @intenet 4G 或 Wifi
 * @data 信息体
 * 无返回
 */
void systemPopup(ENUM_Internet_TypeDef internet, char *data)
{
	u8 port = 0;
	OS_ERR err;
	char *buf = mymalloc(50);
	memset(buf, '\0', 50);
	char *tem = strtok(data, "-");
	port = atoi(tem);
	if (curPort != port)
	{
		curPort = port;
		popUP_powerBank(port + 1);
		PowerbankSTA.currentStatuCode[port] = checkPowerbankStatus(port);
		getPowerbankSTAStrWithoutRSSI(buf, 100, UP_SystemPopupSTA, 1,
				PowerbankSTA.powerBankBuf[port]);

		if (internet == In4G)
		{
			Module4G_Send(buf);
		}
		else if (internet == InWifi)
		{
			WIFI_Send(buf);
		}
		OSTmrStart(&Stmr3, &err); //3S后允许弹出同一卡口
	}
	myfree(buf);
}

/**
 * 订单弹出充电宝
 * @intenet 4G 或 Wifi
 * @fram 数据帧结构体
 * @params 参数结构体
 * 无返回
 */
void orderPopup(ENUM_Internet_TypeDef internet, char *data)
{
	u8 port = 0;
	OS_ERR err;
	char *dd = mymalloc(50);
	char *buf = mymalloc(150);
	memset(dd, '\0', 50);
	memset(buf, '\0', 150);
	char *tem = strtok(data, "-");
	port = atoi(tem);
	tem = strtok(NULL, "-");
	strcpy(dd, tem);

	if (curPort != port)
	{
		curPort = port;
		popUP_powerBank(port + 1);
		PowerbankSTA.currentStatuCode[port] = checkPowerbankStatus(port);
		getPowerbankSTAStrWithoutRSSI(buf, 150, UP_OrderPopupPowerbank, 2,
				PowerbankSTA.powerBankBuf[port], dd);

		if (internet == In4G)
		{
			Module4G_Send(buf);
		}
		else if (internet == InWifi)
		{
			WIFI_Send(buf);
		}
		OSTmrStart(&Stmr3, &err); //3S后允许弹出同一卡口
	}
	myfree(dd);
	myfree(buf);
}
/**
 * 修改锁的状态
 * @data 信息体
 * 无返回
 */
void modifyLockSTA(char *data)
{
	//修改忽略某卡口锁异常状态
	//接口参数：卡口_状态（1表示写死，0表示正常读取
	char *tem = strtok(data, "-");
	u8 kk = atoi(tem);
	tem = strtok(NULL, "-");
	u8 ksta = atoi(tem);
	MyFlashParams.IgnoreLock[kk] = ksta;
	MyFlash_Write(&MyFlashParams);
}
/**
 * 普通心跳
 */
void commonHeart(ENUM_Internet_TypeDef internet)
{
	if (internet == In4G)
	{
		Module4G_Send(Heart);
	}
	else if (internet == InWifi)
	{
		WIFI_Send(Heart);
	}
}
/**
 * 强制心跳
 * @internet 端口
 * @upCMD
 */
void forceHeart(ENUM_Internet_TypeDef internet, ENUM_tcpUP_TypeDef upCmd,
		FunctionalState state)
{
	char *buf;
	if (state)
	{
		//查询所有卡口状态
		for (int i = 0; i < 12; i++)
		{
			PowerbankSTA.currentStatuCode[i] = checkPowerbankStatus(i);
		}
	}
	buf = mymalloc(512);
	memset(buf, '\0', 512);
	getPowerbankSTAStr(buf, 512, upCmd, TCP_Params.rssi, 12,
			PowerbankSTA.powerBankBuf[0], PowerbankSTA.powerBankBuf[1],
			PowerbankSTA.powerBankBuf[2], PowerbankSTA.powerBankBuf[3],
			PowerbankSTA.powerBankBuf[4], PowerbankSTA.powerBankBuf[5],
			PowerbankSTA.powerBankBuf[6], PowerbankSTA.powerBankBuf[7],
			PowerbankSTA.powerBankBuf[8], PowerbankSTA.powerBankBuf[9],
			PowerbankSTA.powerBankBuf[10], PowerbankSTA.powerBankBuf[11]);

	if (internet == In4G)
	{
		Module4G_Send(buf);
	}
	else if (internet == InWifi)
	{
		WIFI_Send(buf);
	}
	myfree(buf);
}
/**
 * 响应系统重启
 */
void responseReset(ENUM_Internet_TypeDef internet)
{
	char *buf = mymalloc(20);
	memset(buf, '\0', 20);
	getRequestStrWithoutParam(buf, 20, UP_DeviceRest);
	if (internet == In4G)
	{
		Module4G_Send(buf);
	}
	else if (internet == InWifi)
	{
		WIFI_Send(buf);
	}
	myfree(buf);
	NVIC_SystemReset();
}
/**
 * 上报当前卡口状态变化
 * @internet 端口
 * @port 0-11
 */
void reportPortStatuChanged(ENUM_Internet_TypeDef internet, u8 port)
{
	char *buf = mymalloc(50);
	memset(buf, '\0', 50);
	getPowerbankSTAStrWithoutRSSI(buf, 50, UP_PowerbankSTAChanged, 1,
			PowerbankSTA.powerBankBuf[port]);

	if (internet == In4G)
	{
		Module4G_Send(buf);
	}
	else if (internet == InWifi)
	{
		WIFI_Send(buf);
	}
	myfree(buf);
}
/**
 * 请求注册
 * @internet 端口
 */
void request4Register(ENUM_Internet_TypeDef internet)
{
	char *buf = mymalloc(100);
	memset(buf, '\0', 100);
	getRegisterStr(buf, 100, UP_Regiser, '2');
	if (internet == In4G)
	{
		Module4G_Send(buf);
	}
	else if (internet == InWifi)
	{
		WIFI_Send(buf);
	}
	myfree(buf);
}
/**
 * 设置wifi名和密码
 * @data 信息体
 */
void setWifiSsidAndPwd(ENUM_Internet_TypeDef internet, char *data)
{
	char *buf = mymalloc(100);
	memset(buf, '\0', 100);
	getWifiSsidAndPwd(data);
	getPowerbankSTAStrWithoutRSSI(buf, 100, UP_SetWifiSsidAndPwd, 1, data);
	if (internet == In4G)
	{
		Module4G_Send(buf);
	}
	else if (internet == InWifi)
	{
		WIFI_Send(buf);
	}
	myfree(buf);
	TCP_Params.wifiParamModified = 1; //标记wifi账号密码修改
}
/**
 * 获取注册参数
 */
void getRegisterParams(ENUM_Internet_TypeDef internet, char *data)
{
	char *tem = NULL;
	tem = strtok(data, "_");
	RegisterParams.heartTime = atoi(tem);
	tem = strtok(NULL, "_");
	RegisterParams.statuHeartTime = atoi(tem);
	DEBUG("ht=%d,sht=%d\r\n", RegisterParams.heartTime,
			RegisterParams.statuHeartTime);
	//上报所有卡口状态
	forceHeart(internet, UP_AllPortsSTA, ENABLE);
	if (internet == In4G)
	{
		F4G_Fram.allowHeart = 1;
		F4G_Fram.firstStatuHeartNotSucc = 1;
	}
	else if (internet == InWifi)
	{
		WIFI_Fram.allowHeart = 1;
		WIFI_Fram.firstStatuHeartNotSucc = 1;
	}
}
/**
 * 远程控制电机转动
 */
void setMotorRun(char *data)
{
	char *ptr = NULL;
	u16 kk, gtime, ktime;
	ptr = strtok(data, "-");
	kk = atoi(ptr);
	ptr = strtok(NULL, "-");
	gtime = atoi(ptr);
	ptr = strtok(NULL, "-");
	ktime = atoi(ptr);
	remoteCtrMotot(kk, gtime, ktime);
}

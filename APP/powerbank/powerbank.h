/*
 * powerbank.h
 *
 *  Created on: 2021年1月6日
 *      Author: HHS007
 */

#ifndef POWERBANK_POWERBANK_H_
#define POWERBANK_POWERBANK_H_

#include "dma.h"
#include <stdarg.h>
#include "malloc.h"
#include "L74HC595.h"
#if SYSTEM_SUPPORT_OS
#include "includes.h"
#endif
#include "task.h"

extern vu8 curPort;

#define BAT_EN1    		(PDout(4))
#define BAT_EN2    		(PDout(5))
#define BAT_EN3    		(PDout(6))
#define BAT_EN4    		(PDout(7))
#define BAT_EN5    		(PDout(15))
#define BAT_EN6    		(PDout(14))
#define BAT_EN7    		(PEout(5))
#define BAT_EN8    		(PEout(4))
#define BAT_EN9    		(PEout(9))
#define BAT_EN10   		(PEout(8))
#define BAT_EN11   		(PEout(7))
#define BAT_EN12   		(PBout(2))
#define _USART6_CTL     (PCout(8))
#define _A0_1           (PBout(3))
#define _A1_1           (PBout(4))
#define _A0_2           (PDout(8))
#define _A1_2           (PDout(9))
#define _A0_3           (PBout(14))
#define _A1_3           (PBout(15))
#define _A0_4           (PCout(2))
#define _A1_4           (PCout(3))

enum BAT_CMD_ENUM
{
	BAT_ComTest = 0xFF,
	BAT_SetID_SUCCESS = 0xFE,
	BAT_Down_Header = 0xA0,
	BAT_Down_End = 0xA1,
	BAT_Down_ReadID = 0x01,
	BAT_Down_SetID = 0x11,
	BAT_Down_ReadVOL = 0x02,
	BAT_Down_ReadCUR = 0x03,
	BAT_Down_ReadERROR = 0x0A,
	BAT_Down_Admin = 0x0F,
	BAT_Up_Header = 0xB0,
	BAT_Up_End = 0xB1,
	BAT_Up_ReadID = 0x81,
	BAT_Up_ReadVOL = 0x82,
	BAT_Up_ReadCUR = 0x83,
	BAT_Up_ReadERROR = 0x8A,
	BAT_Up_Admin = 0x8F
};

struct Powerbank_Params_TypeDef
{
	u8 VOL;
	u8 ERROR;
	u16 CUR;
	u8 Charging[12];
	vu8 ChargingCnt;
	u8 BatID[9];
	char powerBankBuf[12][18];
};
extern struct Powerbank_Params_TypeDef PowerbankSTA;

#define USART6_RX_BUF_LEN  32
#define USART6_TX_BUF_LEN  32

struct USART6_Fram_TypeDef
{
	u8 RxBuf[USART6_RX_BUF_LEN];
	u8 TxBuf[USART6_TX_BUF_LEN];
	vu16 AccessLen;
	vu8 DMA_Tx_Busy;
};
extern struct USART6_Fram_TypeDef USART6_Fram;


void Charge_Init(void);
unsigned int get_crc(unsigned char pbuf[], unsigned char num);
unsigned int get_crc_2(int num, ...);
void app_cmd_anasys(u8 *data, u8 cmd);
void app_frame_anasys(u8 *data, u16 size);
u16 fillDataToTxBuf(u8 cmd, u8 allowSetID, u8 *id);
void setBATInstruction(u8 port, u8 sta);
void controlPowerBankCharge(u8 pb, u8 allow);
void USART6_Init(u32 bound);
void communicateWithPort(u8 port);
void Powerbank_Init(u32 bound);

void USART6_DMA_Send(u16 size);

#endif /* POWERBANK_POWERBANK_H_ */

/*
 * mySwitch.h
 *
 *  Created on: 2021年1月5日
 *      Author: HHS007
 */

#ifndef SWITCH_MYSWITCH_H_
#define SWITCH_MYSWITCH_H_
#include "sys.h"

void Switch_Init(void);
u8 getBatSwitchSTA(u8 bat);
u8 getMotorSwitchSTA(u8 mot);

#endif /* SWITCH_MYSWITCH_H_ */

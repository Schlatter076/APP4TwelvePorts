/*
 * wifi.h
 *
 *  Created on: 2021年1月8日
 *      Author: HHS007
 */

#ifndef WIFI_WIFI_H_
#define WIFI_WIFI_H_

#include "task.h"

#define _WIFI_RST   (PAout(1))


void WIFI_Init(u32 bound);
#endif /* WIFI_WIFI_H_ */

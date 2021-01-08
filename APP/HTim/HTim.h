/*
 * HTim.h
 *
 *  Created on: 2021年1月7日
 *      Author: HHS007
 */

#ifndef HTIM_HTIM_H_
#define HTIM_HTIM_H_

#include "backlight.h"
#include "tcp_public.h"
#if SYSTEM_SUPPORT_OS
#include "includes.h"
#endif

void TIM3_Init(u16 arr, u16 psc);

#endif /* HTIM_HTIM_H_ */

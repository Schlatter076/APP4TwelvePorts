/*
 * usart.h
 *
 *  Created on: 2021年1月5日
 *      Author: HHS007
 */

#ifndef USART_H_
#define USART_H_

#include "sys.h"
#include "tcp_public.h"
#if SYSTEM_SUPPORT_OS
#include "includes.h"
#endif
#include "STMFlash.h"
#include "dma.h"
#include "task.h"
#include <stdarg.h>
#include <stdio.h>

void USART1_Init(u32 bound);
void Debug_Printf(const char *fmt, ...);



#endif /* USART_H_ */

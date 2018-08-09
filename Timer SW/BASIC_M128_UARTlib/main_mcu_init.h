#ifndef _MAIN_MCU_INIT_H_
	#define _MAIN_MCU_INIT_H_

#include "main_config.h"
#include<avr/wdt.h>
#include "avr_defines.h"
#include "main_defines.h"
#include "main_func.h"
#include "avr_usart.h"

void watchdog_init(void);

void port_init(void);

void init_devices(void);

#endif /* _MAIN_MCU_INIT_H_ */

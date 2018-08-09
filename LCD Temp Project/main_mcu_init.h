#ifndef _MAIN_MCU_INIT_H_
	#define _MAIN_MCU_INIT_H_

#include "main_config.h"
#include<avr/wdt.h>
#include "avr_defines.h"
#include "main_defines.h"
#include "avr_usart.h"
#include "avr_twi_i2c.h"
#include "avr_timere_sw.h"
#include "avr_adc.h"


void watchdog_init(void);

void port_init(void);

void init_devices(void);

#endif /* _MAIN_MCU_INIT_H_ */

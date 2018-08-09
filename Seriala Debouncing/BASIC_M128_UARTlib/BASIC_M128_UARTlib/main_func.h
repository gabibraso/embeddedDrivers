/*
* main.c
*
* Created: 22.05.2018
* Author : Bogdan Tarnauca
*/

#ifndef _MAIN_FUNC_H_
	#define _MAIN_FUNC_H_
	

//Apps must include an "main_config.h" header where libraries related params, overrides, settings should be defined.
//This header file is application specific. Must be provided even it is empty.
#include "main_config.h"
	
#include <stdio.h>	

#include "avr_defines.h"
#include "main_defines.h"

void ledToggle(void);

#endif
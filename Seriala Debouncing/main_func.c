/*
* main.c
*
* Created: 22.05.2018
* Author : Bogdan Tarnauca
*/

#include "main_func.h"

/**
*** @fn      ledToggle(void)
***
*** @brief   Toggle the LED
***
*** @param   none
*** @return  none.
**/
void ledToggle(void)
{
	 *ledPort ^= ledPinMask;
}

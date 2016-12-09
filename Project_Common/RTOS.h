/**
 * \file
 * \brief RTOS interface.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module the main tasks of the RTOS.
 */

#ifndef RTOS_H_
#define RTOS_H_

#include "Platform.h"
#if PL_CONFIG_HAS_RTOS

void led1Neg(void);

typedef struct {
	unsigned short blinkFrequency;
	void (*callbackFunct)(void);
} led_t;

/*! \brief Initializes the RTOS module */
void RTOS_Init(void);

/*! \brief De-Initializes the RTOS module */
void RTOS_Deinit(void);

#endif /* PL_CONFIG_HAS_RTOS */

#endif /* RTOS_H_ */

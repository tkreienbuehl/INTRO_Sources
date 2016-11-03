/**
 * \file
 * \brief Real Time Operating System (RTOS) main program.
 * \author Erich Styger, erich.styger@hslu.ch
 */

#include "Platform.h"
#if PL_CONFIG_HAS_RTOS
#include "RTOS.h"
#include "FRTOS1.h"
#include "LED.h"
#include "Event.h"
#include "Keys.h"
#include "Application.h"

void led1Neg(void) {
	LED1_Neg();
}

void led2Neg(void) {
	LED2_Neg();
}

static void AppTask(void* param) {

  const led_t* led = (led_t*)param;

  (void)param; /* avoid compiler warning */
  for(;;) {
	led->callbackFunct();
    FRTOS1_vTaskDelay(pdMS_TO_TICKS(led->blinkFrequency));
  }
}

void RTOS_Init(void) {
  static led_t led1;
  static led_t led2;

  led1.blinkFrequency = 200;
  led1.callbackFunct = led1Neg;

  led2.blinkFrequency = 500;
  led2.callbackFunct = led2Neg;

  EVNT_SetEvent(EVNT_STARTUP); /* set startup event */
  /*! \todo Create tasks here */
  if (FRTOS1_xTaskCreate(AppTask, (signed portCHAR *)"App1", configMINIMAL_STACK_SIZE, (void*)&led1, tskIDLE_PRIORITY, NULL) != pdPASS) {
	  for(;;){} /* error case only, stay here! */
  }
  if (FRTOS1_xTaskCreate(AppTask, (signed portCHAR *)"App2", configMINIMAL_STACK_SIZE, (void*)&led2, tskIDLE_PRIORITY, NULL) != pdPASS) {
	  for(;;){}
  }
}

void RTOS_Deinit(void) {
  /* nothing needed for now */
}

#endif /* PL_CONFIG_HAS_RTOS */

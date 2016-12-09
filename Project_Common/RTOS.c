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
#include "Shell.h"
#if PL_CONFIG_HAS_KEYS
	#if PL_CONFIG_HAS_DEBOUNCE
		#include "KeyDebounce.h"
	#endif
#endif

void led1Neg(void) {
	LED1_Neg();
}

static void EventHandleTask(void* param) {
	(void)param;		//no parameters needed
	for (;;) {
	#if PL_CONFIG_HAS_EVENTS
		EVNT_HandleEvent(APP_EventHandler, TRUE);
	#endif

	#if PL_CONFIG_HAS_KEYS
		#if PL_CONFIG_HAS_DEBOUNCE
		  KEYDBNC_Process();
		#else
		  KEY_Scan();
		#endif
	#endif
		FRTOS1_vTaskDelay(pdMS_TO_TICKS(50));
	}
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

  led1.blinkFrequency = 1000;
  led1.callbackFunct  = led1Neg;

  EVNT_SetEvent(EVNT_STARTUP); /* set startup event */

  /*! \todo Create tasks here */
  if (FRTOS1_xTaskCreate(AppTask, (portCHAR*)"App1", configMINIMAL_STACK_SIZE, (void*)&led1, tskIDLE_PRIORITY, NULL) != pdPASS) {
	  for(;;){} /* error case only, stay here! */
  }
  //SHELL_SendString((uint8_t*)"AppTask for LED 1 created");
  if (FRTOS1_xTaskCreate(EventHandleTask, (portCHAR*)"Event_Handler", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL) != pdPASS) {
	  for(;;){}
  }
  //SHELL_SendString((uint8_t*)"AppTask for LED 2 created");
}

void RTOS_Deinit(void) {
  /* nothing needed for now */
}

#endif /* PL_CONFIG_HAS_RTOS */

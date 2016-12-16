/**
 * \file
 * \brief Main application file
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This provides the main application entry point.
 */

#include "Platform.h"
#include "Application.h"
#include "Event.h"
#include "LED.h"
#include "WAIT1.h"
#include "CS1.h"
#include "Keys.h"
#include "KIN1.h"
#include "HF1.h"
#include "Timer.h"
#include "SW1.h"
#if PL_CONFIG_HAS_SHELL
  #include "CLS1.h"
  #include "Shell.h"
#endif
#if PL_CONFIG_HAS_BUZZER
  #include "Buzzer.h"
#endif
#if PL_CONFIG_HAS_RTOS
  #include "FRTOS1.h"
  #include "RTOS.h"
#endif
#if PL_CONFIG_HAS_QUADRATURE
  #include "Q4CLeft.h"
  #include "Q4CRight.h"
#endif
#if PL_CONFIG_HAS_MOTOR
  #include "Motor.h"
#endif
#if PL_CONFIG_BOARD_IS_ROBO_V2
  #include "PORT_PDD.h"
#endif
#if PL_CONFIG_HAS_LINE_FOLLOW
	#include "LineFollow.h"
#endif
#if PL_CONFIG_HAS_LCD
	#include "LCD.h"
#endif

#if PL_CONFIG_HAS_EVENTS
void APP_EventHandler(EVNT_Handle event) {
  switch(event) {
  case EVNT_STARTUP:
    LED1_On(); /* just do something */
#if PL_CONFIG_HAS_BUZZER
    BUZ_PlayTune(BUZ_TUNE_WELCOME);
#endif
    break;
  case EVNT_LED_HEARTBEAT:
    LED1_Neg();
    break;
#if PL_CONFIG_HAS_KEYS
	#if PL_CONFIG_NOF_KEYS>=1
	case EVNT_SW1_PRESSED:
		LED2_Neg();
		SHELL_SendString((uint8_t*)"SW1 pressed\r\n");
		#if PL_CONFIG_HAS_BUZZER
			BUZ_PlayTune(BUZ_TUNE_BUTTON);
		#endif
		#if PL_CONFIG_HAS_LCD_MENU
			EVNT_SetEvent(EVNT_LCD_BTN_RIGHT);
		#endif
		#if PL_CONFIG_HAS_SNAKE_GAME
			EVNT_SetEvent(EVNT_SNAKE_RIGHT);
		#endif
		break;
	case EVNT_SW1_RELEASED:
		SHELL_SendString((uint8_t*)"SW1 released\r\n");
		#if PL_CONFIG_BOARD_IS_ROBO
			LF_StartStopFollowing();
		#endif
		break;
	#if PL_CONFIG_NOF_KEYS>=2
	case EVNT_SW2_PRESSED:
		SHELL_SendString((uint8_t*)"SW2 pressed\r\n");
		#if PL_CONFIG_HAS_BUZZER
			BUZ_PlayTune(BUZ_TUNE_BUTTON);
		#endif
		#if PL_CONFIG_HAS_LCD_MENU
			EVNT_SetEvent(EVNT_LCD_BTN_LEFT);
		#endif
		#if PL_CONFIG_HAS_SNAKE_GAME
			EVNT_SetEvent(EVNT_SNAKE_LEFT);
		#endif
		break;
	#endif
	#if PL_CONFIG_NOF_KEYS>=3
	case EVNT_SW3_PRESSED:
		//CLS1_SendStr((const uint8*)"SW2 pressed\r\n", CLS1_GetStdio()->stdOut);
		SHELL_SendString((uint8_t*)"SW3 pressed\r\n");
		#if PL_CONFIG_HAS_BUZZER
			BUZ_PlayTune(BUZ_TUNE_BUTTON);
		#endif
		#if PL_CONFIG_HAS_LCD_MENU
			EVNT_SetEvent(EVNT_LCD_BTN_DOWN);
		#endif
		#if PL_CONFIG_HAS_SNAKE_GAME
			EVNT_SetEvent(EVNT_SNAKE_DOWN);
		#endif
	  break;
	#endif
	#if PL_CONFIG_NOF_KEYS>=4
	case EVNT_SW4_PRESSED:
		//CLS1_SendStr((const uint8*)"SW2 pressed\r\n", CLS1_GetStdio()->stdOut);
		SHELL_SendString((uint8_t*)"SW4 pressed\r\n");
		#if PL_CONFIG_HAS_BUZZER
			BUZ_PlayTune(BUZ_TUNE_BUTTON);
		#endif
		#if PL_CONFIG_HAS_LCD_MENU
			EVNT_SetEvent(EVNT_LCD_BTN_CENTER);
		#endif
		#if PL_CONFIG_HAS_SNAKE_GAME
			EVNT_SetEvent(EVNT_SNAKE_START_PAUSE);
		#endif
		break;
	case EVNT_SW4_PRESSED:
		//CLS1_SendStr((const uint8*)"SW2 pressed\r\n", CLS1_GetStdio()->stdOut);
		SHELL_SendString((uint8_t*)"SW4 pressed\r\n");
		#if PL_CONFIG_HAS_BUZZER
			BUZ_PlayTune(BUZ_TUNE_BUTTON);
		#endif
		#if PL_CONFIG_HAS_LCD_MENU
			EVNT_SetEvent(EVNT_LCD_BTN_CENTER);
		#endif
		#if PL_CONFIG_HAS_SNAKE_GAME
			EVNT_SetEvent(EVNT_SNAKE_START_PAUSE);
		#endif
		break;
	case EVNT_SW4_LPRESSED:
		//CLS1_SendStr((const uint8*)"SW2 pressed\r\n", CLS1_GetStdio()->stdOut);
		SHELL_SendString((uint8_t*)"SW4 long pressed\r\n");
		#if PL_CONFIG_HAS_LCD_MENU
			EVNT_SetEvent(EVNT_LCD_BTN_CENTER_LONG);
		#endif
		break;
	#endif
	#if PL_CONFIG_NOF_KEYS>=5
	case EVNT_SW5_PRESSED:
		//CLS1_SendStr((const uint8*)"SW2 pressed\r\n", CLS1_GetStdio()->stdOut);
		SHELL_SendString((uint8_t*)"SW5 pressed\r\n");
		#if PL_CONFIG_HAS_BUZZER
			BUZ_PlayTune(BUZ_TUNE_BUTTON);
		#endif
		#if PL_CONFIG_HAS_LCD_MENU
			EVNT_SetEvent(EVNT_LCD_BTN_UP);
		#endif
		#if PL_CONFIG_HAS_SNAKE_GAME
			EVNT_SetEvent(EVNT_SNAKE_UP);
		#endif
		break;
	#endif
	#if PL_CONFIG_NOF_KEYS>=6
	case EVNT_SW6_PRESSED:
		//CLS1_SendStr((const uint8*)"SW2 pressed\r\n", CLS1_GetStdio()->stdOut);
		SHELL_SendString((uint8_t*)"SW6 pressed\r\n");
		#if PL_CONFIG_HAS_BUZZER
		BUZ_PlayTune(BUZ_TUNE_BUTTON);
		#endif
		#if PL_CONFIG_HAS_LCD_MENU
			EVNT_SetEvent(EVNT_LCD_SIDE_BTN_DOWN);
		#endif
		break;
	case EVNT_SW6_LPRESSED:
		SHELL_SendString((uint8_t*)"SW6 long pressed\r\n");
		#if PL_CONFIG_HAS_BUZZER
		BUZ_PlayTune(BUZ_TUNE_BUTTON);
		#endif
		#if PL_CONFIG_HAS_LCD_MENU
			EVNT_SetEvent(EVNT_LCD_SIDE_BTN_DOWN_LONG);
		#endif
		break;
	#endif
	#if PL_CONFIG_NOF_KEYS>=7
	case EVNT_SW7_PRESSED:
		//CLS1_SendStr((const uint8*)"SW2 pressed\r\n", CLS1_GetStdio()->stdOut);
		SHELL_SendString((uint8_t*)"SW7 pressed\r\n");
		#if PL_CONFIG_HAS_BUZZER
		BUZ_PlayTune(BUZ_TUNE_BUTTON);
		#endif
		#if PL_CONFIG_HAS_LCD_MENU
			EVNT_SetEvent(EVNT_LCD_SIDE_BTN_UP);
		#endif
		break;
	case EVNT_SW7_LPRESSED:
		SHELL_SendString((uint8_t*)"SW7 long pressed\r\n");
		#if PL_CONFIG_HAS_BUZZER
		BUZ_PlayTune(BUZ_TUNE_BUTTON);
		#endif
		#if PL_CONFIG_HAS_LCD_MENU
			EVNT_SetEvent(EVNT_LCD_SIDE_BTN_UP_LONG);
		#endif
		break;
	#endif
  #endif
#endif /* PL_CONFIG_HAS_KEYS */
  	default:
  	break;
    /* \todo extend handler as needed */
   } /* switch */
}
#endif /* PL_CONFIG_HAS_EVENTS */

static const KIN1_UID RoboIDs[] = {
  /* 0: L20, V2 */ {{0x00,0x03,0x00,0x00,0x4E,0x45,0xB7,0x21,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}},
  /* 1: L21, V2 */ {{0x00,0x05,0x00,0x00,0x4E,0x45,0xB7,0x21,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}},
  /* 2: L4, V1  */ {{0x00,0x0B,0xFF,0xFF,0x4E,0x45,0xFF,0xFF,0x4E,0x45,0x27,0x99,0x10,0x02,0x00,0x24}}, /* revert right motor */
};

static void APP_AdoptToHardware(void) {
  KIN1_UID id;
  uint8_t res;

  res = KIN1_UIDGet(&id);
  if (res!=ERR_OK) {
    for(;;); /* error */
  }
#if PL_CONFIG_HAS_MOTOR
  if (KIN1_UIDSame(&id, &RoboIDs[2])) { /* L4 */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* revert left motor */
    (void)Q4CLeft_SwapPins(TRUE);
    (void)Q4CRight_SwapPins(TRUE);
  }
#endif
#if PL_CONFIG_HAS_QUADRATURE && PL_CONFIG_BOARD_IS_ROBO_V2
  /* pull-ups for Quadrature Encoder Pins */
  PORT_PDD_SetPinPullSelect(PORTC_BASE_PTR, 10, PORT_PDD_PULL_UP);
  PORT_PDD_SetPinPullEnable(PORTC_BASE_PTR, 10, PORT_PDD_PULL_ENABLE);
  PORT_PDD_SetPinPullSelect(PORTC_BASE_PTR, 11, PORT_PDD_PULL_UP);
  PORT_PDD_SetPinPullEnable(PORTC_BASE_PTR, 11, PORT_PDD_PULL_ENABLE);
  PORT_PDD_SetPinPullSelect(PORTC_BASE_PTR, 16, PORT_PDD_PULL_UP);
  PORT_PDD_SetPinPullEnable(PORTC_BASE_PTR, 16, PORT_PDD_PULL_ENABLE);
  PORT_PDD_SetPinPullSelect(PORTC_BASE_PTR, 17, PORT_PDD_PULL_UP);
  PORT_PDD_SetPinPullEnable(PORTC_BASE_PTR, 17, PORT_PDD_PULL_ENABLE);
#endif
}

void APP_Start(void) {
#if PL_CONFIG_HAS_RTOS
#if configUSE_TRACE_HOOKS
  PTRC1_uiTraceStart();
#endif
#endif
  PL_Init();
#if PL_CONFIG_HAS_EVENTS
  EVNT_SetEvent(EVNT_STARTUP);
#endif
#if CLS1_DEFAULT_SERIAL
  CLS1_SendStr((const uint8_t*)"Hello World!\r\n", CLS1_GetStdio()->stdOut);
#endif
#if PL_CONFIG_HAS_SHELL_QUEUE
  SHELL_SendString((uint8_t*)"Welcome!");
#endif
#if PL_CONFIG_HAS_KEYS
    KEY_Init();
#endif
  APP_AdoptToHardware();
#if PL_CONFIG_HAS_RTOS
  vTaskStartScheduler(); /* start the RTOS, create the IDLE task and run my tasks (if any) */
  /* does usually not return! */
#else
  for(;;) {
#if PL_CONFIG_HAS_KEYS
	#if PL_CONFIG_HAS_DEBOUNCE
	  KEYDBNC_Process();
	#else
	  KEY_Scan();
	#endif
#endif
#if PL_CONFIG_HAS_EVENTS
    EVNT_HandleEvent(APP_EventHandler, TRUE);
#endif
   // EVNT_SetEvent((EVNT_Handle)EVNT_LED_HEARTBEAT);
    WAIT1_Waitms(25); /* just wait for some arbitrary time .... */
  }
#endif
}



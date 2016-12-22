#include "Platform.h"
#if PL_CONFIG_BOARD_IS_REMOTE
#include "AppHandler.h"
#include "RApp.h"
#include "LCD.h"
#include "LCDMenu.h"

static bool searchStartField = FALSE;
static APPHDL_AppIDs currentApp = APPHDL_APP_ID_NONE;

void eventButtonUp(void) {
	switch (currentApp) {
	case APPHDL_APP_ID_ROBO:
		(void)RAPP_SendPayloadDataBlock((uint8_t*)"0", sizeof("0")-1, RAPP_MSG_TYPE_INC_SPD, RNETA_GetDestAddr(), 0L);
		LCD_PrintSWPressed((uint8_t*)"<driving>\nfaster");
		break;
	default:
		break;
	}
}

void eventButtonDown(void) {
	switch (currentApp) {
	case APPHDL_APP_ID_ROBO:
		(void)RAPP_SendPayloadDataBlock((uint8_t*)"0", sizeof("0")-1, RAPP_MSG_TYPE_DEC_SPD, RNETA_GetDestAddr(), 0L);
		LCD_PrintSWPressed((uint8_t*)"<driving>\nslower");
		break;
	default:
		break;
	}
}

void eventButtonRight(void) {
	switch (currentApp) {
	case APPHDL_APP_ID_ROBO:
		(void)RAPP_SendPayloadDataBlock((uint8_t*)"0", sizeof("0")-1, RAPP_MSG_TYPE_INC_RIGHT, RNETA_GetDestAddr(), 0L);
		LCD_PrintSWPressed((uint8_t*)"<driving> \nturn right");
		break;
	default:
		break;
	}
}

void eventButtonLeft(void) {
	switch (currentApp) {
	case APPHDL_APP_ID_ROBO:
		(void)RAPP_SendPayloadDataBlock((uint8_t*)"0", sizeof("0")-1, RAPP_MSG_TYPE_INC_LEFT, RNETA_GetDestAddr(), 0L);
		LCD_PrintSWPressed((uint8_t*)"<driving> \nturn left");
		break;
	default:
		break;
	}
}

void eventButtonCenterSchort(void) {
	switch (currentApp) {
	case APPHDL_APP_ID_ROBO:
		(void)RAPP_SendPayloadDataBlock((uint8_t*)"0", sizeof("0")-1, RAPP_MSG_TYPE_ZERO_DIR, RNETA_GetDestAddr(), 0L);
		LCD_PrintSWPressed((uint8_t*)"<driving> \nstraight");
		break;
	default:
		break;
	}
}

void eventButtonCenterLong(void) {
	switch (currentApp) {
	case APPHDL_APP_ID_ROBO:
		searchStartField = !searchStartField;
		if (searchStartField) {
			(void)RAPP_SendPayloadDataBlock((uint8_t*)"1", sizeof("1")-1, RAPP_MSG_TYPE_SEARCH_START, RNETA_GetDestAddr(), 0L);
			LCD_PrintSWPressed((uint8_t*)"<driving> \nstart searching\nstart field");
		}
		else {
			(void)RAPP_SendPayloadDataBlock((uint8_t*)"0", sizeof("0")-1, RAPP_MSG_TYPE_SEARCH_START, RNETA_GetDestAddr(), 0L);
			LCD_PrintSWPressed((uint8_t*)"<driving> \nstop searching\nstart field");
		}
		break;
	default:
		break;
	}
}

void eventButtonRightUp(void) {
	switch (currentApp) {
	case APPHDL_APP_ID_ROBO:
		(void)RAPP_SendPayloadDataBlock((uint8_t*)"0", sizeof("0")-1, RAPP_MSG_TYPE_TURN_LEFT, RNETA_GetDestAddr(), 0L);
		LCD_PrintSWPressed((uint8_t*)"<driving> \nturn 90 deg \nto left");
		break;
	default:
		break;
	}
}

void eventButtonRightUpLong(void) {
	switch (currentApp) {
	case APPHDL_APP_ID_ROBO:
		(void)RAPP_SendPayloadDataBlock((uint8_t*)"0", sizeof("0")-1, RAPP_MSG_TYPE_STOP, RNETA_GetDestAddr(), 0L);
		LCD_PrintSWPressed((uint8_t*)"<driving> \nstopped");
		break;
	default:
		break;
	}
}

void eventButtonRightDown(void) {
	switch (currentApp) {
	case APPHDL_APP_ID_ROBO:
		(void)RAPP_SendPayloadDataBlock((uint8_t*)"0", sizeof("0")-1, RAPP_MSG_TYPE_TURN_RIGHT, RNETA_GetDestAddr(), 0L);
		LCD_PrintSWPressed((uint8_t*)"<driving> \nturn 90 deg \nto right");
		break;
	default:
		break;
	}
}

void eventEnterApplication(void) {
	switch (currentApp) {
	case APPHDL_APP_ID_ROBO:
		(void)RAPP_SendPayloadDataBlock((uint8_t*)"0", sizeof("0")-1, RAPP_MSG_TYPE_START_DRIVE, RNETA_GetDestAddr(), 0L);
		LCD_PrintSWPressed((uint8_t*)"<driving> \nready");
		break;
	default:
		break;
	}
	APPHDL_isInAppMode = TRUE;
}

void eventExitApplication(void) {
	switch (currentApp) {
	case APPHDL_APP_ID_ROBO:
		APPHDL_isInAppMode = FALSE;
		break;
	default:
		break;
	}
	currentApp = APPHDL_APP_ID_NONE;
}

void APPHDL_SetAppID(APPHDL_AppIDs id) {
	if (currentApp != APPHDL_APP_ID_NONE) {	//End last App first
		eventExitApplication();
	}
	currentApp = id;
}

void APPHDL_OnEvent(APPHDL_EventTypes evnt) {
	switch (evnt) {
	case APPHDL_EVENT_BTN_UP:
		eventButtonUp();
		break;
	case APPHDL_EVENT_BTN_DOWN:
		eventButtonDown();
		break;
	case APPHDL_EVENT_BTN_RIGHT:
		eventButtonRight();
		break;
	case APPHDL_EVENT_BTN_LEFT:
		eventButtonLeft();
		break;
	case APPHDL_EVENT_BTN_CENTER:
		eventButtonCenterSchort();
		break;
	case APPHDL_EVENT_BTN_CENTER_LONG:
		eventButtonCenterLong();
		break;
	case APPHDL_EVENT_BNT_RIGHT_UP:
		eventButtonRightUp();
		break;
	case APPHDL_EVENT_BNT_RIGHT_UP_LONG:
		eventButtonRightUpLong();
		break;
	case APPHDL_EVENT_BTN_RIGHT_DOWN:
		eventButtonRightDown();
		break;
	case APPHDL_EVENT_ENTER_APPLICATION:
		eventEnterApplication();
		break;
	case APPHDL_EVENT_EXIT_APPLICATION:
		eventExitApplication();
		break;
	default:
		break;
	}
}

void APPHDL_init(void) {
	APPHDL_isInAppMode = FALSE;
}

#endif //PL_CONFIG_BOARD_IS_REMOTE

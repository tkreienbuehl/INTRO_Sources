/**
 * \file
 * \brief Module to handle the LCD display
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module is driving the Nokia LCD Display.
 */

#include "Platform.h"
#if PL_CONFIG_HAS_LCD
#include "LCD.h"
#include "PDC1.h"
#include "GDisp1.h"
#include "GFONT1.h"
#include "FDisp1.h"
#include "Application.h"
#include "UTIL1.h"
#include "LCD_LED.h"
#include "Event.h"
#include "FRTOS1.h"
#include "RApp.h"
#include "LCDMenu.h"
#if PL_CONFIG_HAS_RADIO
#include "RNet_App.h"
#endif

/* status variables */
static bool LedBackLightisOn = TRUE;
static bool requestLCDUpdate = FALSE;

#if PL_CONFIG_HAS_LCD_MENU
typedef enum {
	LCD_MENU_ID_NONE = LCDMENU_ID_NONE, /* special value! */
	LCD_MENU_ID_MAIN,
	LCD_MENU_ID_BACKLIGHT,
	LCD_MENU_ID_NUM_VALUE,
	LCD_MENU_ID_ROBO_MAIN,
	LCD_MENU_ID_ROBO_DRIVE,
	LCD_MENU_ID_ROBO_MUSIC,
	LCD_MENU_ID_PLAY_TUNE,
	LCD_MENU_ID_PLAY_HAPPY,
	LCD_MENU_ID_ROBO_SETTINGS,
	LCD_MENU_ID_GAMES,
	LCD_MENU_ID_TETRIS,
	LCD_MENU_ID_SNAKE,
} LCD_MenuIDs;

static LCDMenu_StatusFlags ValueChangeHandler(const struct LCDMenu_MenuItem_ *item, LCDMenu_EventType event, void **dataP) {
  static int value = 0;
  static uint8_t valueBuf[16];
  LCDMenu_StatusFlags flags = LCDMENU_STATUS_FLAGS_NONE;

  (void)item;
  if (event==LCDMENU_EVENT_GET_TEXT) {
    UTIL1_strcpy(valueBuf, sizeof(valueBuf), (uint8_t*)"Val: ");
    UTIL1_strcatNum32s(valueBuf, sizeof(valueBuf), value);
    *dataP = valueBuf;
    flags |= LCDMENU_STATUS_FLAGS_HANDLED|LCDMENU_STATUS_FLAGS_UPDATE_VIEW;
  } else if (event==LCDMENU_EVENT_GET_EDIT_TEXT) {
    UTIL1_strcpy(valueBuf, sizeof(valueBuf), (uint8_t*)"[-] ");
    UTIL1_strcatNum32s(valueBuf, sizeof(valueBuf), value);
    UTIL1_strcat(valueBuf, sizeof(valueBuf), (uint8_t*)" [+]");
    *dataP = valueBuf;
    flags |= LCDMENU_STATUS_FLAGS_HANDLED|LCDMENU_STATUS_FLAGS_UPDATE_VIEW;
  } else if (event==LCDMENU_EVENT_DECREMENT) {
    value--;
    flags |= LCDMENU_STATUS_FLAGS_HANDLED|LCDMENU_STATUS_FLAGS_UPDATE_VIEW;
  } else if (event==LCDMENU_EVENT_INCREMENT) {
    value++;
    flags |= LCDMENU_STATUS_FLAGS_HANDLED|LCDMENU_STATUS_FLAGS_UPDATE_VIEW;
  }
  return flags;
}

static LCDMenu_StatusFlags DriveHandler(const struct LCDMenu_MenuItem_ *item, LCDMenu_EventType event, void **dataP) {
  static int value = 0;
  static uint8_t valueBuf[16];
  LCDMenu_StatusFlags flags = LCDMENU_STATUS_FLAGS_NONE;

  (void)item;
  if (event==LCDMENU_EVENT_GET_TEXT) {
    UTIL1_strcpy(valueBuf, sizeof(valueBuf), (uint8_t*)"Val: ");
    UTIL1_strcatNum32s(valueBuf, sizeof(valueBuf), value);
    *dataP = valueBuf;
    flags |= LCDMENU_STATUS_FLAGS_HANDLED|LCDMENU_STATUS_FLAGS_UPDATE_VIEW;
  } else if (event==LCDMENU_EVENT_INC_UP) {

    *dataP = valueBuf;
    flags |= LCDMENU_STATUS_FLAGS_HANDLED|LCDMENU_STATUS_FLAGS_UPDATE_VIEW;
  } else if (event==LCDMENU_EVENT_DEC_DOWN) {
	  (void)RAPP_SendPayloadDataBlock((uint8_t*)"0", sizeof("0")-1, RAPP_MSG_TYPE_DEC_SPD, RNETA_GetDestAddr(), RPHY_PACKET_FLAGS_REQ_ACK);
    flags |= LCDMENU_STATUS_FLAGS_HANDLED|LCDMENU_STATUS_FLAGS_UPDATE_VIEW;
  } else if (event==LCDMENU_EVENT_LEFT_INC) {

    flags |= LCDMENU_STATUS_FLAGS_HANDLED|LCDMENU_STATUS_FLAGS_UPDATE_VIEW;
  }else if (event==LCDMENU_EVENT_RIGHT_INC) {

      flags |= LCDMENU_STATUS_FLAGS_HANDLED|LCDMENU_STATUS_FLAGS_UPDATE_VIEW;
    }
  return flags;
}

static LCDMenu_StatusFlags BackLightMenuHandler(const struct LCDMenu_MenuItem_ *item, LCDMenu_EventType event, void **dataP) {
  LCDMenu_StatusFlags flags = LCDMENU_STATUS_FLAGS_NONE;

  (void)item;
  if (event==LCDMENU_EVENT_GET_TEXT && dataP!=NULL) {
    if (LedBackLightisOn) {
      *dataP = "Backlight is ON";
    } else {
      *dataP = "Backlight is OFF";
    }
    flags |= LCDMENU_STATUS_FLAGS_HANDLED|LCDMENU_STATUS_FLAGS_UPDATE_VIEW;
  } else if (event==LCDMENU_EVENT_ENTER) { /* toggle setting */
    LedBackLightisOn = !LedBackLightisOn;
    flags |= LCDMENU_STATUS_FLAGS_HANDLED|LCDMENU_STATUS_FLAGS_UPDATE_VIEW;
  }
  return flags;
}

static LCDMenu_StatusFlags PlayMusicMenuHandler(const struct LCDMenu_MenuItem_ *item, LCDMenu_EventType event, void **dataP) {
	LCDMenu_StatusFlags flags = LCDMENU_STATUS_FLAGS_NONE;
	(void)item;

	if (event==LCDMENU_EVENT_GET_TEXT) {
		if (item->id == LCD_MENU_ID_PLAY_TUNE) {
			*dataP = "Play tune";
		}
		else {
			*dataP = "Play happy";
		}
		flags |= LCDMENU_STATUS_FLAGS_HANDLED|LCDMENU_STATUS_FLAGS_UPDATE_VIEW;
	}
	else if (event==LCDMENU_EVENT_ENTER) { /* toggle setting */
		if (item->id == LCD_MENU_ID_PLAY_TUNE) {
			(void)RAPP_SendPayloadDataBlock((uint8_t*)"0", sizeof("0")-1, RAPP_MSG_TYPE_PLAY_TUNE, RNETA_GetDestAddr(), RPHY_PACKET_FLAGS_REQ_ACK);
		}
		else {
			(void)RAPP_SendPayloadDataBlock((uint8_t*)"0", sizeof("0")-1, RAPP_MSG_TYPE_PLAY_TUNE, RNETA_GetDestAddr(), RPHY_PACKET_FLAGS_REQ_ACK);
		}
		flags |= LCDMENU_STATUS_FLAGS_HANDLED|LCDMENU_STATUS_FLAGS_UPDATE_VIEW;
	}
	return flags;
}

static const LCDMenu_MenuItem menus[] =
{/* id,								grp,pos,up,						down,						text,				callback					flags                  */
    {LCD_MENU_ID_MAIN,				0,	0,	LCD_MENU_ID_NONE,       LCD_MENU_ID_BACKLIGHT,		"Remote Settings",  NULL,                       LCDMENU_MENU_FLAGS_NONE},
    	{LCD_MENU_ID_BACKLIGHT,		1,	0,	LCD_MENU_ID_MAIN,       LCD_MENU_ID_NONE,			NULL,           	BackLightMenuHandler,       LCDMENU_MENU_FLAGS_NONE},
    	{LCD_MENU_ID_NUM_VALUE,		1,	1,	LCD_MENU_ID_MAIN,       LCD_MENU_ID_NONE,			NULL,           	ValueChangeHandler,         LCDMENU_MENU_FLAGS_EDITABLE},
	{LCD_MENU_ID_ROBO_MAIN,			0,	1,	LCD_MENU_ID_NONE,		LCD_MENU_ID_ROBO_DRIVE,		"Herbie menu",      NULL,						LCDMENU_MENU_FLAGS_NONE},
		{LCD_MENU_ID_ROBO_DRIVE,	2,	0,	LCD_MENU_ID_ROBO_MAIN,	LCD_MENU_ID_NONE,			NULL,				NULL,						LCDMENU_MENU_FLAGS_DRIVEABLE},
		{LCD_MENU_ID_ROBO_MUSIC,	2,	1,	LCD_MENU_ID_ROBO_MAIN,	LCD_MENU_ID_PLAY_TUNE,		"Music",			NULL,						LCDMENU_MENU_FLAGS_NONE},
			{LCD_MENU_ID_PLAY_TUNE,	4,	0,	LCD_MENU_ID_ROBO_MUSIC,	LCD_MENU_ID_NONE,			NULL,				PlayMusicMenuHandler,		LCDMENU_MENU_FLAGS_NONE},
			{LCD_MENU_ID_PLAY_HAPPY,4,	1,	LCD_MENU_ID_ROBO_MUSIC,	LCD_MENU_ID_NONE,			NULL,				PlayMusicMenuHandler,		LCDMENU_MENU_FLAGS_NONE},
		{LCD_MENU_ID_ROBO_SETTINGS,	2,	2,	LCD_MENU_ID_ROBO_MAIN,	LCD_MENU_ID_NONE,			NULL,				NULL,						LCDMENU_MENU_FLAGS_NONE},
	{LCD_MENU_ID_GAMES,				0,	2,	LCD_MENU_ID_NONE,		LCD_MENU_ID_TETRIS,			"Games",			NULL,						LCDMENU_MENU_FLAGS_NONE},
		{LCD_MENU_ID_TETRIS,		3,	0,	LCD_MENU_ID_GAMES,		LCD_MENU_ID_NONE,			"Tetris",			NULL,						LCDMENU_MENU_FLAGS_NONE},
		{LCD_MENU_ID_SNAKE,			3,	1,	LCD_MENU_ID_GAMES,		LCD_MENU_ID_NONE,			"Snake",			NULL,						LCDMENU_MENU_FLAGS_NONE},
};

uint8_t LCD_HandleRemoteRxMessage(RAPP_MSG_Type type, uint8_t size, uint8_t *data, RNWK_ShortAddrType srcAddr, bool *handled, RPHY_PacketDesc *packet) {
  (void)size;
  (void)packet;
  (void)data;
  (void)srcAddr;
  (void)handled;
  switch(type) {
     default:
      break;
  } /* switch */
  return ERR_OK;
}
#endif /* PL_CONFIG_HAS_LCD_MENU */

/*
static void DrawLines(void) {
  int i;
  GDisp1_PixelDim x, y, w, h;

  GDisp1_Clear();
  GDisp1_UpdateFull();

  GDisp1_DrawBox(0, 0, 10, 10, 1, GDisp1_COLOR_BLACK);
  GDisp1_UpdateFull();
  vTaskDelay(pdMS_TO_TICKS(500));

  GDisp1_DrawBox(GDisp1_GetWidth()-10, 0, 10, 10, 1, GDisp1_COLOR_BLACK);
  GDisp1_UpdateFull();
  vTaskDelay(pdMS_TO_TICKS(500));

  GDisp1_DrawLine(0, 0, GDisp1_GetWidth(), GDisp1_GetHeight(), GDisp1_COLOR_BLACK);
  GDisp1_UpdateFull();
  vTaskDelay(pdMS_TO_TICKS(500));
  GDisp1_DrawLine(0, GDisp1_GetHeight(), GDisp1_GetWidth(), 0, GDisp1_COLOR_BLACK);
  GDisp1_UpdateFull();
  vTaskDelay(pdMS_TO_TICKS(500));
  for(i=0;i<10;i++) {
    GDisp1_DrawCircle(GDisp1_GetWidth()/2, GDisp1_GetHeight()/2, 5+i*2, GDisp1_COLOR_BLACK);
    GDisp1_UpdateFull();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
*/

/*
static void DrawCircles(void) {
	GDisp1_PixelDim x, y;
	static uint8_t xPosMax = GDisp1_GetWidth()-5;
	static uint8_t yPosMax = GDisp1_GetHeight()-5;

	LCD_LED_On();
	GDisp1_Clear();
	GDisp1_UpdateFull();

	for (int i=0; i<10; i++) {
		do {
			x = (GDisp1_PixelDim)rand()+5;
		}
		while(x > xPosMax);
		do {
			y = (GDisp1_PixelDim)rand()+5;
		}
		while(y>yPosMax);
		GDisp1_DrawCircle(x,y,5,GDisp1_COLOR_BLACK);
		GDisp1_UpdateFull();
		vTaskDelay(pdMS_TO_TICKS(250));
		GDisp1_Clear();
		GDisp1_UpdateFull();
	}
}

static void DrawFont(void) {
  FDisp1_PixelDim x, y;

  GDisp1_Clear();
  GDisp1_UpdateFull();
  x = 0;
  y = 10;
  FDisp1_WriteString("Hello World!", GDisp1_COLOR_BLACK, &x, &y, GFONT1_GetFont());
  GDisp1_UpdateFull();
  vTaskDelay(pdMS_TO_TICKS(500));
  x = 0;
  y += GFONT1_GetBoxHeight();
  FDisp1_WriteString("with Fonts!", GDisp1_COLOR_BLACK, &x, &y, GFONT1_GetFont());
  GDisp1_UpdateFull();
  WAIT1_Waitms(1000);
}

*/

static void ShowTextOnLCD(unsigned char *text) {
  FDisp1_PixelDim x, y;

  GDisp1_Clear();
  x = 0;
  y = 10;
  FDisp1_WriteString(text, GDisp1_COLOR_BLACK, &x, &y, GFONT1_GetFont());
  GDisp1_UpdateFull();
}

static void drawWheel(GDisp1_PixelDim x, GDisp1_PixelDim y, uint8_t idx) {

	GDisp1_DrawCircle(x, y, 3, GDisp1_COLOR_BLACK);
	GDisp1_DrawCircle(x, y, 4, GDisp1_COLOR_BLACK);
	GDisp1_DrawCircle(x, y, 5, GDisp1_COLOR_BLACK);

	if (idx == 0) {
		GDisp1_DrawHLine((GDisp1_PixelDim)(x-4), y, (GDisp1_PixelDim)8, GDisp1_COLOR_BLACK);
		GDisp1_DrawVLine(x, (GDisp1_PixelDim)(y-4), (GDisp1_PixelDim)8, GDisp1_COLOR_BLACK);
	}
	else {
		GDisp1_DrawLine((GDisp1_PixelDim)(x-3), (GDisp1_PixelDim)(y+3), (GDisp1_PixelDim)(x+3), (GDisp1_PixelDim)(y-3), GDisp1_COLOR_BLACK);
		GDisp1_DrawLine((GDisp1_PixelDim)(x-3), (GDisp1_PixelDim)(y-3), (GDisp1_PixelDim)(x+3), (GDisp1_PixelDim)(y+3), GDisp1_COLOR_BLACK);
	}

}

static void drawCar(GDisp1_PixelDim x, GDisp1_PixelDim y) {

	GDisp1_DrawHLine((GDisp1_PixelDim)(x+7),(GDisp1_PixelDim)(y+2),2, GDisp1_COLOR_BLACK);
	GDisp1_DrawVLine((GDisp1_PixelDim)(x+9),(GDisp1_PixelDim)(y-5),7, GDisp1_COLOR_BLACK);
	GDisp1_DrawHLine((GDisp1_PixelDim)(x+5),(GDisp1_PixelDim)(y-5),4, GDisp1_COLOR_BLACK);
	GDisp1_DrawLine((GDisp1_PixelDim)(x+5),(GDisp1_PixelDim)(y-5),x,(GDisp1_PixelDim)(y-10), GDisp1_COLOR_BLACK);
	GDisp1_DrawLine(x,(GDisp1_PixelDim)(y-10),(GDisp1_PixelDim)(x-16),(GDisp1_PixelDim)(y-10), GDisp1_COLOR_BLACK);
	GDisp1_DrawLine((GDisp1_PixelDim)(x-16),(GDisp1_PixelDim)(y-10),(GDisp1_PixelDim)(x-24),(GDisp1_PixelDim)(y-4), GDisp1_COLOR_BLACK);
	GDisp1_DrawLine((GDisp1_PixelDim)(x-24),(GDisp1_PixelDim)(y-4),(GDisp1_PixelDim)(x-28),(GDisp1_PixelDim)(y-3), GDisp1_COLOR_BLACK);
	GDisp1_DrawLine((GDisp1_PixelDim)(x-28),(GDisp1_PixelDim)(y-3),(GDisp1_PixelDim)(x-29),(GDisp1_PixelDim)(y+2), GDisp1_COLOR_BLACK);
	GDisp1_DrawLine((GDisp1_PixelDim)(x-29),(GDisp1_PixelDim)(y+2),(GDisp1_PixelDim)(x-22),(GDisp1_PixelDim)(y+2),GDisp1_COLOR_BLACK);

	GDisp1_DrawLine((GDisp1_PixelDim)(x-14),(GDisp1_PixelDim)(y+2),(GDisp1_PixelDim)(x-5),(GDisp1_PixelDim)(y+2),GDisp1_COLOR_BLACK);

	GDisp1_DrawLine((GDisp1_PixelDim)(x-10),(GDisp1_PixelDim)(y-8),(GDisp1_PixelDim)(x-15),(GDisp1_PixelDim)(y-8), GDisp1_COLOR_BLACK);
	GDisp1_DrawLine((GDisp1_PixelDim)(x-15),(GDisp1_PixelDim)(y-8),(GDisp1_PixelDim)(x-19),(GDisp1_PixelDim)(y-6), GDisp1_COLOR_BLACK);
	GDisp1_DrawLine((GDisp1_PixelDim)(x-19),(GDisp1_PixelDim)(y-6),(GDisp1_PixelDim)(x-10),(GDisp1_PixelDim)(y-6), GDisp1_COLOR_BLACK);
	GDisp1_DrawLine((GDisp1_PixelDim)(x-10),(GDisp1_PixelDim)(y-6),(GDisp1_PixelDim)(x-10),(GDisp1_PixelDim)(y-8), GDisp1_COLOR_BLACK);

	GDisp1_DrawLine((GDisp1_PixelDim)(x+3),(GDisp1_PixelDim)(y-6),(GDisp1_PixelDim)(x-2),(GDisp1_PixelDim)(y-8), GDisp1_COLOR_BLACK);
	GDisp1_DrawLine((GDisp1_PixelDim)(x-2),(GDisp1_PixelDim)(GDisp1_PixelDim)(y-8),(GDisp1_PixelDim)(x-8),(GDisp1_PixelDim)(y-8), GDisp1_COLOR_BLACK);
	GDisp1_DrawLine((GDisp1_PixelDim)(x-8),(GDisp1_PixelDim)(y-8),(GDisp1_PixelDim)(x-8),(GDisp1_PixelDim)(y-6), GDisp1_COLOR_BLACK);
	GDisp1_DrawLine((GDisp1_PixelDim)(x-8),(GDisp1_PixelDim)(y-6),(GDisp1_PixelDim)(x+3),(GDisp1_PixelDim)(y-6), GDisp1_COLOR_BLACK);

}

static void DrawWelcomeScreen(void) {
	GDisp1_PixelDim x, y, pos, cnt;
	static GDisp1_PixelDim xPosMax = GDisp1_GetWidth()-12;
	static GDisp1_PixelDim yPosMax = GDisp1_GetHeight()-8;

	x = (GDisp1_PixelDim)(xPosMax + 30);
	y = yPosMax;
	cnt = 0;

	LCD_LED_On();

	while (x > 29) {

		GDisp1_Clear();
		GDisp1_UpdateFull();

		pos = (GDisp1_PixelDim)(cnt % 2);

		drawWheel(x, (GDisp1_PixelDim)(y+1),pos);
		drawWheel((GDisp1_PixelDim)(x-18), (GDisp1_PixelDim)(y+1),pos);
		drawCar(x,(GDisp1_PixelDim)(y-pos));

		GDisp1_UpdateFull();

		vTaskDelay(pdMS_TO_TICKS(150));
		x=(GDisp1_PixelDim)(x-2);
		cnt++;

	}

	x = 0;
	y = 10;
	FDisp1_WriteString((uint8_t*)"Welcome!", GDisp1_COLOR_BLACK, &x, &y, GFONT1_GetFont());
	GDisp1_UpdateFull();
	vTaskDelay(pdMS_TO_TICKS(2000));

}

static void LCD_Task(void *param) {
  (void)param; /* not used */
#if 1
  DrawWelcomeScreen();
#endif
#if PL_CONFIG_HAS_LCD_MENU
  LCDMenu_InitMenu(menus, sizeof(menus)/sizeof(menus[0]), 1);
  LCDMenu_OnEvent(LCDMENU_EVENT_DRAW, NULL);
#endif
  for(;;) {
    if (LedBackLightisOn) {
      LCD_LED_On(); /* LCD backlight on */
    } else {
      LCD_LED_Off(); /* LCD backlight off */
    }
#if PL_CONFIG_HAS_LCD_MENU
    if (requestLCDUpdate) {
      requestLCDUpdate = FALSE;
      LCDMenu_OnEvent(LCDMENU_EVENT_DRAW, NULL);
    }
#if 1 /*! \todo Change this to for your own needs, or use direct task notification */
    if (EVNT_EventIsSetAutoClear(EVNT_LCD_BTN_LEFT)) { /* left */
      LCDMenu_OnEvent(LCDMENU_EVENT_LEFT, NULL);
      //ShowTextOnLCD("left");
    }
    if (EVNT_EventIsSetAutoClear(EVNT_LCD_BTN_RIGHT)) { /* right */
      LCDMenu_OnEvent(LCDMENU_EVENT_RIGHT, NULL);
      //ShowTextOnLCD("right");
    }
    if (EVNT_EventIsSetAutoClear(EVNT_LCD_BTN_UP)) { /* up */
      LCDMenu_OnEvent(LCDMENU_EVENT_UP, NULL);
      //ShowTextOnLCD("up");
    }
    if (EVNT_EventIsSetAutoClear(EVNT_LCD_BTN_DOWN)) { /* down */
      LCDMenu_OnEvent(LCDMENU_EVENT_DOWN, NULL);
      //ShowTextOnLCD("down");
    }
    if (EVNT_EventIsSetAutoClear(EVNT_LCD_BTN_CENTER)) { /* center */
      LCDMenu_OnEvent(LCDMENU_EVENT_ENTER, NULL);
      //(void)RAPP_SendPayloadDataBlock((uint8_t*)"F", sizeof("F")-1, RAPP_MSG_TYPE_JOYSTICK_BTN, RNETA_GetDestAddr(), RPHY_PACKET_FLAGS_REQ_ACK);
      //ShowTextOnLCD("center");
    }
    if (EVNT_EventIsSetAutoClear(EVNT_LCD_SIDE_BTN_UP)) { /* side up */
      LCDMenu_OnEvent(LCDMENU_EVENT_UP, NULL);
      //ShowTextOnLCD("side up");
    }
    if (EVNT_EventIsSetAutoClear(EVNT_LCD_SIDE_BTN_DOWN)) { /* side down */
      LCDMenu_OnEvent(LCDMENU_EVENT_DOWN, NULL);
      //ShowTextOnLCD("side down");
    }
#endif
#endif /* PL_CONFIG_HAS_LCD_MENU */
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

void LCD_PrintSWPressed(uint8_t* string) {
	ShowTextOnLCD(string);
}

void LCD_Deinit(void) {
#if PL_CONFIG_HAS_LCD_MENU
  LCDMenu_Deinit();
#endif
}

void LCD_Init(void) {
  LedBackLightisOn =  TRUE;
  //if (xTaskCreate(LCD_Task, "LCD", configMINIMAL_STACK_SIZE+30, NULL, tskIDLE_PRIORITY, &LCDTaskHandle) != pdPASS) {
  if (xTaskCreate(LCD_Task, "LCD", configMINIMAL_STACK_SIZE+30, NULL, tskIDLE_PRIORITY, NULL) != pdPASS) {
    for(;;){} /* error! probably out of memory */
  }
#if PL_CONFIG_HAS_LCD_MENU
  LCDMenu_Init();
#endif
}
#endif /* PL_CONFIG_HAS_LCD */

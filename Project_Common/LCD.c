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
	LCD_MENU_ID_ROBO_LINE,
	LCD_MENU_ID_ROBO_MUSIC,
	LCD_MENU_ID_PLAY_TUNE,
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

static const LCDMenu_MenuItem menus[] =
{/* id,								grp,pos,up,						down,						text,				callback					flags                  */
    {LCD_MENU_ID_MAIN,				0,	0,	LCD_MENU_ID_NONE,       LCD_MENU_ID_BACKLIGHT,		"Remote Settings",  NULL,                       LCDMENU_MENU_FLAGS_NONE},
    	{LCD_MENU_ID_BACKLIGHT,		1,	0,	LCD_MENU_ID_MAIN,       LCD_MENU_ID_NONE,			NULL,           	BackLightMenuHandler,       LCDMENU_MENU_FLAGS_NONE},
    	{LCD_MENU_ID_NUM_VALUE,		1,	1,	LCD_MENU_ID_MAIN,       LCD_MENU_ID_NONE,			NULL,           	ValueChangeHandler,         LCDMENU_MENU_FLAGS_EDITABLE},
	{LCD_MENU_ID_ROBO_MAIN,			0,	1,	LCD_MENU_ID_NONE,		LCD_MENU_ID_ROBO_DRIVE,		"Herbie menu",      NULL,						LCDMENU_MENU_FLAGS_NONE},
		{LCD_MENU_ID_ROBO_DRIVE,	2,	0,	LCD_MENU_ID_ROBO_MAIN,	LCD_MENU_ID_NONE,			"Drive Herbie",		NULL,						LCDMENU_MENU_FLAGS_NONE},
		{LCD_MENU_ID_ROBO_LINE,		2,	1,	LCD_MENU_ID_ROBO_MAIN,	LCD_MENU_ID_NONE,			NULL,				NULL,						LCDMENU_MENU_FLAGS_NONE},
		{LCD_MENU_ID_ROBO_MUSIC,	2,	2,	LCD_MENU_ID_ROBO_MAIN,	LCD_MENU_ID_PLAY_TUNE,		"Music",			NULL,						LCDMENU_MENU_FLAGS_NONE},
			{LCD_MENU_ID_PLAY_TUNE,	4,	0,	LCD_MENU_ID_ROBO_MUSIC,	LCD_MENU_ID_NONE,			"Play tune",		NULL,						LCDMENU_MENU_FLAGS_NONE},
		{LCD_MENU_ID_ROBO_SETTINGS,	2,	3,	LCD_MENU_ID_ROBO_MAIN,	LCD_MENU_ID_NONE,			NULL,				NULL,						LCDMENU_MENU_FLAGS_NONE},
	{LCD_MENU_ID_GAMES,				0,	2,	LCD_MENU_ID_NONE,		LCD_MENU_ID_TETRIS,			"Games",			NULL,						LCDMENU_MENU_FLAGS_NONE},
		{LCD_MENU_ID_TETRIS,		3,	0,	LCD_MENU_ID_GAMES,		LCD_MENU_ID_NONE,			"Tetris",			NULL,						LCDMENU_MENU_FLAGS_NONE},
		{LCD_MENU_ID_SNAKE,			3,	1,	LCD_MENU_ID_GAMES,		LCD_MENU_ID_NONE,			"Snake",			NULL,						LCDMENU_MENU_FLAGS_NONE},
};

uint8_t LCD_HandleRemoteRxMessage(RAPP_MSG_Type type, uint8_t size, uint8_t *data, RNWK_ShortAddrType srcAddr, bool *handled, RPHY_PacketDesc *packet) {
  (void)size;
  (void)packet;
  switch(type) {
     default:
      break;
  } /* switch */
  return ERR_OK;
}
#endif /* PL_CONFIG_HAS_LCD_MENU */

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

static void DrawCircles(void) {
	GDisp1_PixelDim x, y;
	static unsigned int xPosMax = GDisp1_GetWidth()-5;
	static unsigned int yPosMax = GDisp1_GetHeight()-5;

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

static void DrawText(void) {
  GDisp1_Clear();
  GDisp1_UpdateFull();
  PDC1_WriteLineStr(1, (uint8_t*)"hello");
  vTaskDelay(pdMS_TO_TICKS(200));
  PDC1_WriteLineStr(2, (uint8_t*)"world");
  vTaskDelay(pdMS_TO_TICKS(200));
  PDC1_WriteLineStr(3, (uint8_t*)"out");
  vTaskDelay(pdMS_TO_TICKS(200));
  PDC1_WriteLineStr(4, (uint8_t*)"there");
  vTaskDelay(pdMS_TO_TICKS(200));
  PDC1_WriteLineStr(5, (uint8_t*)"!!!!!");
  vTaskDelay(pdMS_TO_TICKS(200));
}

static void ShowTextOnLCD(unsigned char *text) {
  FDisp1_PixelDim x, y;

  GDisp1_Clear();
  x = 0;
  y = 10;
  FDisp1_WriteString(text, GDisp1_COLOR_BLACK, &x, &y, GFONT1_GetFont());
  GDisp1_UpdateFull();
}

static void LCD_Task(void *param) {
  (void)param; /* not used */
#if 1
  ShowTextOnLCD((uint8_t*)"Press a key!");
  DrawText();
  /* \todo extend */
  //DrawFont();
  //DrawLines();
  DrawCircles();
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
      //RSTDIO_SendToTxStdio(RSTDIO_QUEUE_TX_IN, "buzzer play tune", sizeof("buzzer play tune")-1);
      (void)RAPP_SendPayloadDataBlock((uint8_t*)"F", sizeof("F")-1, RAPP_MSG_TYPE_JOYSTICK_BTN, RNETA_GetDestAddr(), RPHY_PACKET_FLAGS_REQ_ACK);
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

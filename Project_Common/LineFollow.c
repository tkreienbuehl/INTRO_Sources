/**
 * \file
 * \brief Implements line following of the robot
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This is the implementation of the line following.
 */

#include "Platform.h"
#if PL_CONFIG_HAS_LINE_FOLLOW
#include "LineFollow.h"
#include "FRTOS1.h"
#include "CLS1.h"
#include "Shell.h"
#include "Motor.h"
#include "Reflectance.h"
#if PL_CONFIG_HAS_TURN
  #include "Turn.h"
#endif
#include "WAIT1.h"
#include "Pid.h"
#include "Drive.h"
#include "Shell.h"
#if PL_CONFIG_HAS_BUZZER
  #include "Buzzer.h"
#endif
#if PL_CONFIG_HAS_DRIVE
  #include "Drive.h"
#endif
#if PL_CONFIG_HAS_LINE_MAZE
  #include "Maze.h"
#endif
#include "RNet_AppConfig.h"
#include "RPHY.h"

typedef enum {
  STATE_IDLE,              /* idle, not doing anything */
  STATE_FOLLOW_SEGMENT,    /* line following segment, going forward */
  STATE_TURN,              /* reached an intersection, turning around */
  STATE_FINISHED,          /* reached finish area */
  STATE_LINE_LEFT,			/*when line is at the left limit*/
  STATE_LINE_RIGHT,		  /*when line is at the right limit*/
  STATE_STOP               /* stop the engines */
} StateType;

/* task notification bits */
#define LF_START_FOLLOWING (1<<0)  /* start line following */
#define LF_STOP_FOLLOWING  (1<<1)  /* stop line following */

static volatile StateType LF_currState = STATE_IDLE;
static xTaskHandle LFTaskHandle;
#if PL_CONFIG_HAS_LINE_MAZE
static uint8_t LF_solvedIdx = 0; /*  index to iterate through the solution, zero is the solution start index */
#endif

static bool enableAutoTurn=false;

void LF_enableAutoTurn(bool enable){
	enableAutoTurn=enable;
}
void LF_StartFollowing(void) {
  (void)xTaskNotify(LFTaskHandle, LF_START_FOLLOWING, eSetBits);
}

void LF_StopFollowing(void) {
  (void)xTaskNotify(LFTaskHandle, LF_STOP_FOLLOWING, eSetBits);
}

void LF_StartStopFollowing(void) {
  if (LF_IsFollowing()) {
    (void)xTaskNotify(LFTaskHandle, LF_STOP_FOLLOWING, eSetBits);
  } else {
    (void)xTaskNotify(LFTaskHandle, LF_START_FOLLOWING, eSetBits);
  }
}


static void StateMachine(void);

/*!
 * \brief follows a line segment.
 * \return Returns TRUE if still on line segment
 */
static REF_LineKind FollowSegment(void) {
  uint16_t currLine;
  REF_LineKind currLineKind;

  currLine = REF_GetLineValue();
  currLineKind = REF_GetLineKind();
  if (currLineKind==REF_LINE_STRAIGHT ||currLineKind==REF_LINE_LEFT||currLineKind==REF_LINE_RIGHT) {
    PID_Line(currLine, REF_MIDDLE_LINE_VALUE); /* move along the line */
  }
  return currLineKind;
}

static void StateMachine(void) {
	REF_LineKind lineKind;
  switch (LF_currState) {
    case STATE_IDLE:
      break;
    case STATE_FOLLOW_SEGMENT:
    	lineKind= FollowSegment();
      if (lineKind == REF_LINE_FULL) {
        LF_currState = STATE_STOP; /* stop if we do not have a line any more */
        SHELL_SendString((unsigned char*)"No line, stopped!\r\n");
        (void)RAPP_SendPayloadDataBlock((uint8_t*)"C", sizeof("C")-1, RAPP_MSG_TYPE_SIGNALS, ADDRESS_SIGNALS, RPHY_PACKET_FLAGS_REQ_ACK);
      }
      else if (lineKind == REF_LINE_NONE) {
		LF_currState = STATE_TURN; /* make turn */
		SHELL_SendString((unsigned char*)"no line, turn..\r\n");
      }
      else if (lineKind == REF_LINE_LEFT) {
		//LF_currState = STATE_LINE_LEFT; /* make turn */
		//SHELL_SendString((unsigned char*)"line left, turn..\r\n");
      }
      else if (lineKind == REF_LINE_RIGHT) {
		//LF_currState = STATE_LINE_RIGHT; /* make turn */
		//SHELL_SendString((unsigned char*)"line right, turn..\r\n");
      }
      break;
    case STATE_LINE_LEFT:
    	TURN_TurnAngle(10,NULL);
    	(void)DRV_SetMode(DRV_MODE_NONE);
    	LF_currState=STATE_FOLLOW_SEGMENT;
    	break;
    case STATE_LINE_RIGHT:
    	TURN_TurnAngle(-10,NULL);
    	(void)DRV_SetMode(DRV_MODE_NONE);
    	LF_currState=STATE_FOLLOW_SEGMENT;
    	break;
    case STATE_TURN:
    	TURN_TurnAngle(180,NULL);
    	(void)DRV_SetMode(DRV_MODE_NONE);
    	LF_currState=STATE_FOLLOW_SEGMENT;
      break;

    case STATE_FINISHED:
        LF_currState = STATE_STOP; /* stop if we do not have a line any more */
        SHELL_SendString((unsigned char*)"No line, finished!\r\n");
      break;
    case STATE_STOP:
      SHELL_SendString("Stopped!\r\n");
#if PL_CONFIG_HAS_TURN
      TURN_Turn(TURN_STOP, NULL);
#endif
      LF_currState = STATE_IDLE;
      break;
  } /* switch */
}

bool LF_IsFollowing(void) {
  return LF_currState!=STATE_IDLE;
}

static void LineTask (void *pvParameters) {
  uint32_t notifcationValue;
  static bool alreadyStarted=false;

  (void)pvParameters; /* not used */
  for(;;) {
    (void)xTaskNotifyWait(0UL, LF_START_FOLLOWING|LF_STOP_FOLLOWING, &notifcationValue, 0); /* check flags */
    if (notifcationValue&LF_START_FOLLOWING) {
      DRV_SetMode(DRV_MODE_NONE); /* disable any drive mode */
      PID_Start();
      LF_currState = STATE_FOLLOW_SEGMENT;
    }
    if (notifcationValue&LF_STOP_FOLLOWING) {
      LF_currState = STATE_STOP;
    }
    //if(LF_currState==STATE_IDLE && !alreadyStarted){
    if(LF_currState==STATE_IDLE && DRV_GetMode()==DRV_MODE_SPEED && enableAutoTurn){
    	  REF_LineKind currLineKind;
    	  currLineKind = REF_GetLineKind();
    	  if(currLineKind==REF_LINE_FULL){
    		  //Todo Send signal B
    		  PID_Start();
    		  (void)RAPP_SendPayloadDataBlock((uint8_t*)"B", sizeof("B")-1, RAPP_MSG_TYPE_SIGNALS, ADDRESS_SIGNALS, RPHY_PACKET_FLAGS_REQ_ACK);
    		  LF_currState=STATE_TURN;	//start fine state
    		  alreadyStarted=true;
    	  }
    }
    StateMachine();
    FRTOS1_vTaskDelay(5/portTICK_PERIOD_MS);
  }
}

static void LF_PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr((unsigned char*)"line", (unsigned char*)"Group of line following commands\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  help|status", (unsigned char*)"Shows line help or status\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  start|stop", (unsigned char*)"Starts or stops line following\r\n", io->stdOut);
}

static void LF_PrintStatus(const CLS1_StdIOType *io) {
  CLS1_SendStatusStr((unsigned char*)"line follow", (unsigned char*)"\r\n", io->stdOut);
  switch (LF_currState) {
    case STATE_IDLE: 
      CLS1_SendStatusStr((unsigned char*)"  state", (unsigned char*)"IDLE\r\n", io->stdOut);
      break;
    case STATE_FOLLOW_SEGMENT: 
      CLS1_SendStatusStr((unsigned char*)"  state", (unsigned char*)"FOLLOW_SEGMENT\r\n", io->stdOut);
      break;
    case STATE_STOP: 
      CLS1_SendStatusStr((unsigned char*)"  state", (unsigned char*)"STOP\r\n", io->stdOut);
      break;
    case STATE_TURN: 
      CLS1_SendStatusStr((unsigned char*)"  state", (unsigned char*)"TURN\r\n", io->stdOut);
      break;
    case STATE_FINISHED: 
      CLS1_SendStatusStr((unsigned char*)"  state", (unsigned char*)"FINISHED\r\n", io->stdOut);
      break;
    default: 
      CLS1_SendStatusStr((unsigned char*)"  state", (unsigned char*)"UNKNOWN\r\n", io->stdOut);
      break;
  } /* switch */
}

uint8_t LF_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {
  uint8_t res = ERR_OK;

  if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_HELP)==0 || UTIL1_strcmp((char*)cmd, (char*)"line help")==0) {
    LF_PrintHelp(io);
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, (char*)"line status")==0) {
    LF_PrintStatus(io);
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)"line start")==0) {
    LF_StartFollowing();
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)"line stop")==0) {
    LF_StopFollowing();
    *handled = TRUE;
  }
  return res;
}

void LF_Deinit(void) {
  /* nothing needed */
}

void LF_Init(void) {
  LF_currState = STATE_IDLE;
  if (xTaskCreate(LineTask, "Line", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &LFTaskHandle) != pdPASS) {
    for(;;){} /* error */
  }
}
#endif /* PL_CONFIG_HAS_LINE_FOLLOW */

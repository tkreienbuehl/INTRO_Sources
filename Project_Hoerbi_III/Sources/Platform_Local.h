/**
 * \file
 * \brief Local project configuration file.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This header file is used to configure the target application.
 * This header file is included by the common platform.h header file
 * This header file uses PL_LOCAL_CONFIG_ prefix.
 */

#ifndef SOURCES_PLATFORM_LOCAL_H_
#define SOURCES_PLATFORM_LOCAL_H_

/* board identification: */
#define PL_LOCAL_CONFIG_BOARD_IS_ROBO     (1) /* I'm the ROBOT board */

/* platform hardware configuration */
#define PL_LOCAL_CONFIG_NOF_LEDS          (2) /* number of LEDs, 0 to 3 */
#define PL_LOCAL_CONFIG_NOF_KEYS          (1) /* number of keys, 0 to 7 */

#if PL_LOCAL_CONFIG_NOF_KEYS>0
  #define PL_LOCAL_CONFIG_KEY_1_ISR         (0) /* if SW1 is using interrupts */
  #define PL_LOCAL_CONFIG_KEY_2_ISR         (0) /* if SW2 is using interrupts */
  #define PL_LOCAL_CONFIG_KEY_3_ISR         (0) /* if SW3 is using interrupts */
  #define PL_LOCAL_CONFIG_KEY_4_ISR         (0) /* if SW4 is using interrupts */
  #define PL_LOCAL_CONFIG_KEY_5_ISR         (0) /* if SW5 is using interrupts */
  #define PL_LOCAL_CONFIG_KEY_6_ISR         (0) /* if SW6 is using interrupts */
  #define PL_LOCAL_CONFIG_KEY_7_ISR         (0) /* if SW7 is using interrupts */
#endif

/* set of defines to disable a functionality: if it is defined, it will disable it in the common part */
//#define PL_LOCAL_CONFIG_HAS_LEDS_DISABLED                 /* disable LEDs */
//#define PL_LOCAL_CONFIG_HAS_EVENTS_DISABLED               /* disable events */
//#define PL_LOCAL_CONFIG_HAS_TIMER_DISABLED                /* disable own timer */
//#define PL_LOCAL_CONFIG_HAS_KEYS_DISABLED                 /* disable key/push buttons */
//#define PL_LOCAL_CONFIG_HAS_TRIGGER_DISABLED              /* disable triggers */
//#define PL_LOCAL_CONFIG_HAS_DEBOUNCE_DISABLED             /* disable debouncing */
//#define PL_LOCAL_CONFIG_HAS_RTOS_DISABLED                 /* disable RTOS usage */
//#define PL_LOCAL_CONFIG_HAS_SHELL_DISABLED                /* disable shell */
//#define PL_LOCAL_CONFIG_HAS_SEGGER_RTT_DISABLED           /* disable Segger RTT */
//#define PL_LOCAL_CONFIG_HAS_SHELL_QUEUE_DISABLED          /* disable shell queue */
#define PL_LOCAL_CONFIG_HAS_SQUEUE_SINGLE_CHAR_DISABLED   /* disable single character support in shell queue */
//#define PL_LOCAL_CONFIG_HAS_SEMAPHORE_DISABLED            /* disable semaphore test module */
//#define PL_LOCAL_CONFIG_HAS_CONFIG_NVM_DISABLED           /* disable NVM storage */
//#define PL_LOCAL_CONFIG_HAS_USB_CDC_DISABLED              /* disable USB CDC */

/* remote controller hardware functionality */
//#define PL_LOCAL_CONFIG_HAS_RADIO_DISABLED                /* disable Radio transceiver */
#define PL_LOCAL_CONFIG_HAS_REMOTE_STDIO_DISABLED         /* disable Std I/O over radio */
#define PL_LOCAL_CONFIG_HAS_REMOTE_DISABLED               /* disable remote controller (sender and receiver) */
#define PL_LOCAL_CONFIG_HAS_CONTROL_SENDER_DISABLED       /* disable that we are the sender (otherwise we are the receiver) */
#define PL_LOCAL_CONFIG_HAS_JOYSTICK_DISABLED             /* disable joystick */
#define PL_LOCAL_CONFIG_HAS_LCD_DISABLED                  /* disable LCD */
#define PL_LOCAL_CONFIG_HAS_LCD_MENU_DISABLED             /* disable LCD menu */

/* robot hardware functionality */
//#define PL_LOCAL_CONFIG_HAS_MOTOR_DISABLED                /* disable motor */
//#define PL_LOCAL_CONFIG_HAS_REFLECTANCE_DISABLED          /* disable IR reflectance sensor */
//#define PL_LOCAL_CONFIG_HAS_QUADRATURE_DISABLED           /* disable quadrature encoder */
//#define PL_LOCAL_CONFIG_HAS_MOTOR_TACHO_DISABLED          /* disable tacho */
#define PL_LOCAL_CONFIG_HAS_MPC4728_DISABLED              /* disable MPC4728 (only for V1 robot) */
#define PL_LOCAL_CONFIG_HAS_QUAD_CALIBRATION_DISABLED     /* disable quadrature calibration (only for V1 robot) */
//#define PL_LOCAL_CONFIG_HAS_PID_DISABLED                  /* disable PID */
//#define PL_LOCAL_CONFIG_HAS_DRIVE_DISABLED                /* disable drive module */
//#define PL_LOCAL_CONFIG_HAS_TURN_DISABLED                 /* disable turning module */
//#define PL_LOCAL_CONFIG_HAS_LINE_FOLLOW_DISABLED          /* disable line following */
#define PL_LOCAL_CONFIG_HAS_LINE_MAZE_DISABLED            /* disable maze solving */
//#define PL_LOCAL_CONFIG_HAS_BLUETOOTH_DISABLED            /* disable Bluetooth */
//#define PL_LOCAL_CONFIG_HAS_BUZZER_DISABLED               /* disable buzzer (only on robot) */

/* individual settings */
#define PL_MOTOR_L_INVERTED FALSE							  /* in motor.h set MOTOR_HAS_INVERT 1 to use this value*/
#define PL_MOTOR_R_INVERTED TRUE							  /* in motor.h set MOTOR_HAS_INVERT 1 to use this value*/

/* PID Values */
#define PL_SPEED_LEFT_P_FACTOR100         (2000)				  /* P-PART */
#define PL_SPEED_LEFT_I_FACTOR100          (200)               /* I-PART */
#define PL_SPEED_LEFT_D_FACTOR100            (0)               /* D-PART */
#define PL_SPEED_LEFT_I_ANTI_WINDUP      (40000)               /* Anti windup for I*/

#define PL_SPEED_RIGHT_P_FACTOR100        (2000)
#define PL_SPEED_RIGHT_I_FACTOR100         (200)
#define PL_SPEED_RIGHT_D_FACTOR100           (0)
#define PL_SPEED_RIGHT_I_ANTI_WINDUP     (40000)

#define PL_LINE_FOLLOW_P_FACTOR100           (200)
#define PL_LINE_FOLLOW_I_FACTOR100           (50)
#define PL_LINE_FOLLOW_D_FACTOR100           (0)
#define PL_LINE_FOLLOW_I_ANTI_WINDUP         (5000)
#define PL_LINE_FOLLOW_MAX_SPEED_PERCENT     (30)				  /* max allowed speed in percent in line following mode*/

#define PL_POS_LEFT_P_FACTOR100              (2000)
#define PL_POS_LEFT_I_FACTOR100              (0)
#define PL_POS_LEFT_D_FACTOR100              (0)
#define PL_POS_LEFT_I_ANTI_WINDUP            (50000)
#define PL_POS_LEFT_MAX_SPEED_PERCENT        (30) 			  /* max allowed speed in percent in positioning mode*/

#define PL_POS_ANG_VALUE_90                  (750)

#endif /* SOURCES_PLATFORM_LOCAL_H_ */

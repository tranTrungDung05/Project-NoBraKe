/**
 ******************************************************************************
 * @author: GaoKong
 * @date:   13/08/2016
 ******************************************************************************
**/

#ifndef __APP_H__
#define __APP_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "ak.h"


#include "app_if.h"
#include "app_eeprom.h"
#include "app_data.h"

/*****************************************************************************/
/* SYSTEM task define
 */
/*****************************************************************************/
/* define timer */
/* define signal */
enum {
	SYSTEM_AK_FLASH_UPDATE_REQ = AK_USER_DEFINE_SIG,
};

/*****************************************************************************/
/* FIRMWARE task define
 */
/*****************************************************************************/
/* define timer */
#define FW_PACKED_TIMEOUT_INTERVAL			(5000)
#define FW_UPDATE_REQ_INTERVAL				(5000)

/* define signal */
enum {
	FW_CRENT_APP_FW_INFO_REQ = AK_USER_DEFINE_SIG,
	FW_CRENT_BOOT_FW_INFO_REQ,
	FW_UPDATE_REQ,
	FW_UPDATE_SM_OK,
	FW_TRANSFER_REQ,
	FW_INTERNAL_UPDATE_APP_RES_OK,
	FW_INTERNAL_UPDATE_BOOT_RES_OK,
	FW_SAFE_MODE_RES_OK,
	FW_UPDATE_SM_BUSY,
	FW_PACKED_TIMEOUT,
	FW_CHECKING_REQ
};

/*****************************************************************************/
/*  LIFE task define
 */
/*****************************************************************************/
/* define timer */
#define AC_LIFE_TASK_TIMER_LED_LIFE_INTERVAL		(1000)

/* define signal */
enum {
	AC_LIFE_SYSTEM_CHECK = AK_USER_DEFINE_SIG,
};

/*****************************************************************************/
/*  SHELL task define
 */
/*****************************************************************************/
/* define timer */

/* define signal */
enum {
	AC_SHELL_LOGIN_CMD = AK_USER_DEFINE_SIG,
	AC_SHELL_REMOTE_CMD,
};

/*****************************************************************************/
/* IF task define
 */
/*****************************************************************************/
/* define timer */
/* define signal */
enum {
	AC_IF_PURE_MSG_IN = AK_USER_DEFINE_SIG,
	AC_IF_PURE_MSG_OUT,
	AC_IF_COMMON_MSG_IN,
	AC_IF_COMMON_MSG_OUT,
	AC_IF_DYNAMIC_MSG_IN,
	AC_IF_DYNAMIC_MSG_OUT,
};

/*****************************************************************************/
/* UART_IF task define
 */
/*****************************************************************************/
/* timer signal */
/* define signal */

enum {
	AC_UART_IF_INIT = AK_USER_DEFINE_SIG,
	AC_UART_IF_PURE_MSG_OUT,
	AC_UART_IF_COMMON_MSG_OUT,
	AC_UART_IF_DYNAMIC_MSG_OUT,
	AC_UART_IF_PURE_MSG_IN,
	AC_UART_IF_COMMON_MSG_IN,
	AC_UART_IF_DYNAMIC_MSG_IN,
};

/*****************************************************************************/
/*  SCREENS task define
 */
/*****************************************************************************/
/* define timer */
#define AC_DISPLAY_INITIAL_INTERVAL									(100)
#define AC_DISPLAY_STARTUP_INTERVAL									(2000)
#define AC_DISPLAY_IDLE_INTERVAL									(15000)
#define AC_DISPLAY_LOGO_INTERVAL									(10000)
#define AC_DISPLAY_WELCOME_ANIM_INTERVAL							(120)
#define AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE_INTERAL				(150)
#define AC_DISPLAY_MINIMUM_SCREEN_RENDER_INTERVAL_MS				(50) /* 50ms => Max 20 FPS */

/* define signal */
enum {
	/* Render screen signals */
	AC_DISPLAY_RENDER_SCREEN = AK_SYS_DEFINE_SIG,

	/* Button signals */
	AC_DISPLAY_BUTTON_MODE_PRESSED,						
	AC_DISPLAY_BUTTON_MODE_LONG_PRESSED,					
	AC_DISPLAY_BUTTON_MODE_RELEASED,
	AC_DISPLAY_BUTTON_UP_PRESSED,							
	AC_DISPLAY_BUTTON_UP_LONG_PRESSED,					
	AC_DISPLAY_BUTTON_UP_RELEASED,
	AC_DISPLAY_BUTTON_DOWN_PRESSED,						
	AC_DISPLAY_BUTTON_DOWN_LONG_PRESSED,					
	AC_DISPLAY_BUTTON_DOWN_RELEASED,

	/* Screen signal */
	AC_DISPLAY_INITIAL = AK_USER_DEFINE_SIG,

	/* Screen Startup signals */
	AC_DISPLAY_SHOW_STARTUP_LOGO,
	AC_DISPLAY_WELCOME_ANIM,

	/* Idle screen signals */
	AC_DISPLAY_SHOW_IDLE,
	AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE, 

	/* Firmware update signals */
	AC_DISPLAY_SHOW_FW_UPDATE,
	AC_DISPLAY_SHOW_FW_UPDATE_ERR,
};

/*****************************************************************************/
/*  Nobrake game 'SCREEN' task define
 */
/*****************************************************************************/
/* define timer */
#define NB_GAME_TIME_TICK_INTERVAL									(70)
#define NB_GAME_LONG_PRESS_MOVE_INTERVAL							(90)
/* define signal */
enum {
	NB_GAME_TIME_TICK = NB_GAME_DEFINE_SIG,
	NB_GAME_MOVE_LEFT_REPEAT,
	NB_GAME_MOVE_RIGHT_REPEAT,
};

/*****************************************************************************/
/* Nobrake game 'TRACK' task define
 */
/*****************************************************************************/
/* timer signal */
/* define signal */

enum {
	NB_GAME_TRACK_INIT = NB_GAME_DEFINE_SIG,
	NB_GAME_TRACK_UPDATE,
};

/*****************************************************************************/
/* Nobrake game 'OBSTACLE' task define
 */
/*****************************************************************************/
/* timer signal */
/* define signal */

enum {
	NB_GAME_OBSTACLE_SETUP = NB_GAME_DEFINE_SIG,
	NB_GAME_OBSTACLE_UPDATE,
	NB_GAME_OBSTACLE_RESET,
};

/*****************************************************************************/
/* Nobrake game 'CAR' task define
 */
/*****************************************************************************/
/* define signal */
enum {
	NB_GAME_CAR_SETUP = NB_GAME_DEFINE_SIG,
	NB_GAME_CAR_UPDATE,
	NB_GAME_CAR_THROTTLE,
	NB_GAME_CAR_THROTTLE_RELEASE,
	NB_GAME_CAR_ACTION_STRAIGHT,
	NB_GAME_CAR_ACTION_LEFT,
	NB_GAME_CAR_ACTION_RIGHT,
	NB_GAME_CAR_FINISH_BRAKE,
	NB_GAME_CAR_RESET,
};


/*****************************************************************************/
/*  app function declare
 */
/*****************************************************************************/
#define APP_MAGIC_NUMBER	0xAABBCCDD
#define APP_VER				{0, 1, 0, 0}

typedef struct {
	uint32_t magic_number;
	uint8_t version[4];
} app_info_t;

extern const app_info_t app_info;

extern void* app_get_boot_share_data();
extern int  main_app();

#ifdef __cplusplus
}
#endif

#endif //__APP_H__

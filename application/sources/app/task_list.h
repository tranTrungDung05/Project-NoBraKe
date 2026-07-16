#ifndef __TASK_LIST_H__
#define __TASK_LIST_H__

#include "ak.h"
#include "task.h"

extern const task_t app_task_table[];
extern const task_polling_t app_task_polling_table[];

/*****************************************************************************/
/*  DECLARE: Internal Task ID
 *  Note: Task id MUST be increasing order.
 */
/*****************************************************************************/
enum {
	/* SYSTEM TASKS */
	TASK_TIMER_TICK_ID,

	/* APP TASKS */
	AC_TASK_SYSTEM_ID,
	AC_TASK_FW_ID,
	AC_TASK_SHELL_ID,
	AC_TASK_LIFE_ID,
	AC_TASK_IF_ID,
	AC_TASK_UART_IF_ID,
	AC_TASK_DISPLAY_ID,

	/* NOBRAKE GAME ID */
	NB_GAME_TRACK_ID,
	NB_GAME_OBSTACLE_ID,
	NB_GAME_CAR_ID,
	NB_GAME_SCREEN_ID,	

	/* LINK */
#if defined (IF_LINK_UART_EN)
	AC_LINK_PHY_ID,
	AC_LINK_MAC_ID,
	AC_LINK_ID,
#endif

	/* EOT task ID */
	AK_TASK_EOT_ID,
};

/*****************************************************************************/
/*  DECLARE: Internal Polling Task ID
 *  Note: Task id MUST be increasing order.
 */
/*****************************************************************************/
enum {
	/* APP TASKS */
	AC_TASK_POLLING_CONSOLE_ID,

	/* EOT polling task ID */
	AK_TASK_POLLING_EOT_ID,
};

/*****************************************************************************/
/*  DECLARE: Task entry point
 */
/*****************************************************************************/
/* APP TASKS */
extern void task_system(ak_msg_t*);
extern void task_fw(ak_msg_t*);
extern void task_shell(ak_msg_t*);
extern void task_life(ak_msg_t*);
extern void task_if(ak_msg_t*);
extern void task_uart_if(ak_msg_t*);
extern void task_display(ak_msg_t*);

/* NOBRAKE GAME HANDLE */
extern void nb_game_track_handle(ak_msg_t*);
extern void nb_game_obstacle_handle(ak_msg_t*);
extern void nb_game_car_handle(ak_msg_t*);
extern void scr_nobrake_game_handle(ak_msg_t*);
extern void scr_game_score_handle(ak_msg_t*);
extern void scr_game_settings_handle(ak_msg_t*);

/* LINK TASK */
extern void task_link_phy(ak_msg_t*);
extern void task_link_mac(ak_msg_t*);
extern void task_link(ak_msg_t*);

/*****************************************************************************/
/*  DECLARE: Task polling
 */
/*****************************************************************************/
extern void task_polling_console();
extern void task_debug_pools();

#endif //__TASK_LIST_H__

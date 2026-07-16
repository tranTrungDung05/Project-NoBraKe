#include "button.h"

#include "app.h"
#include "app_bsp.h"
#include "task_list.h"
#include "timer.h"

button_t btn_mode;
button_t btn_up;
button_t btn_down;

void btn_mode_callback(void* b) {
	button_t* me_b = (button_t*)b;
	switch (me_b->state) {
	case BUTTON_SW_STATE_PRESSED: {
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_BUTTON_MODE_PRESSED);

		// Reset timer show idle screen
		timer_set(	AC_TASK_DISPLAY_ID, \
					AC_DISPLAY_SHOW_IDLE, \
					AC_DISPLAY_IDLE_INTERVAL, \
					TIMER_ONE_SHOT);
	} break;

	case BUTTON_SW_STATE_LONG_PRESSED: {
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_BUTTON_MODE_LONG_PRESSED);
	} break;

	case BUTTON_SW_STATE_RELEASED: {
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_BUTTON_MODE_RELEASED);
	} break;

	default:
		break;
	}
}

void btn_up_callback(void* b) {
	button_t* me_b = (button_t*)b;
	switch (me_b->state) {
	case BUTTON_SW_STATE_PRESSED: {
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_BUTTON_UP_PRESSED);

		// Reset timer show idle screen
		timer_set(	AC_TASK_DISPLAY_ID, \
					AC_DISPLAY_SHOW_IDLE, \
					AC_DISPLAY_IDLE_INTERVAL, \
					TIMER_ONE_SHOT);
	} break;

	case BUTTON_SW_STATE_LONG_PRESSED: {
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_BUTTON_UP_LONG_PRESSED);
	} break;

	case BUTTON_SW_STATE_RELEASED: {
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_BUTTON_UP_RELEASED);
	} break;

	default:
		break;
	}
}

void btn_down_callback(void* b) {
	button_t* me_b = (button_t*)b;
	switch (me_b->state) {
	case BUTTON_SW_STATE_PRESSED: {
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_BUTTON_DOWN_PRESSED);

		// Reset timer show idle screen
		timer_set(	AC_TASK_DISPLAY_ID, \
					AC_DISPLAY_SHOW_IDLE, \
					AC_DISPLAY_IDLE_INTERVAL, \
					TIMER_ONE_SHOT);
	} break;

	case BUTTON_SW_STATE_LONG_PRESSED: {
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_BUTTON_DOWN_LONG_PRESSED);
	}	
		break;

	case BUTTON_SW_STATE_RELEASED: {
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_BUTTON_DOWN_RELEASED);
	} break;

	default:
		break;
	}
}

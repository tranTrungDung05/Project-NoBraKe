#include "scr_startup.h"

/*****************************************************************************/
/* View - startup */
/*****************************************************************************/
static void view_scr_startup();

view_dynamic_t dyn_view_startup = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_startup
};

view_screen_t scr_startup = {
	&dyn_view_startup,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

void view_scr_startup() {
#define AK_LOGO_AXIS_X		(23)
#define AK_LOGO_TEXT		(AK_LOGO_AXIS_X + 4)
	/* ak logo */
	view_render.clear();
	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	view_render.setCursor(AK_LOGO_AXIS_X, 3);
	view_render.print("   __    _  _ ");
	view_render.setCursor(AK_LOGO_AXIS_X, 10);
	view_render.print("  /__\\  ( )/ )");
	view_render.setCursor(AK_LOGO_AXIS_X, 20);
	view_render.print(" /(__)\\ (   (");
	view_render.setCursor(AK_LOGO_AXIS_X, 30);
	view_render.print("(__)(__)(_)\\_)");
	view_render.setCursor(AK_LOGO_TEXT, 42);
	view_render.print("Active Kernel");
	view_render.update();
}

/*****************************************************************************/
/* Handle - startup */
/*****************************************************************************/
void scr_startup_handle(ak_msg_t* msg) {
	switch (msg->sig) {
	case AC_DISPLAY_INITIAL: {
		APP_DBG_SIG("AC_DISPLAY_INITIAL\n");
		view_render.initialize();
		view_render_display_on();
		timer_set(	AC_TASK_DISPLAY_ID, \
					AC_DISPLAY_SHOW_STARTUP_LOGO, \
					AC_DISPLAY_STARTUP_INTERVAL, \
					TIMER_ONE_SHOT);
	} break;

	case AC_DISPLAY_BUTTON_MODE_PRESSED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_PRESSED\n");
		SCREEN_TRAN(scr_welcome_handle, &scr_welcome);
	} break;

	case AC_DISPLAY_SHOW_STARTUP_LOGO: {
		APP_DBG_SIG("AC_DISPLAY_SHOW_STARTUP_LOGO\n");
		SCREEN_TRAN(scr_welcome_handle, &scr_welcome);
	} break;

	default:
		break;
	}
}

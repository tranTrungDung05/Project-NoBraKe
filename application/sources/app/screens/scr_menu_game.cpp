#include "scr_menu_game.h"
#include "app.h"
#include "screens.h"
#include "screens_bitmap.h"
#include "timer.h"

/*****************************************************************************/
/* Variable and Struct Declaration - Menu game */
/*****************************************************************************/
#define NUMBER_MENU_ITEMS				(4)
#define	SCREEN_MENU_H					(64)
#define MENU_VISIBLE_ITEMS				(3)
#define MENU_ICON_W						(16)
#define MENU_ICON_H						(16)
#define MENU_CHOOSE_ICON_W				(16)
#define MENU_CHOOSE_ICON_H				(10)

// Menu items name
static const char * const menu_items_name[NUMBER_MENU_ITEMS] = {
	"   Play           ",		// item 1
	"   Settings       ",		// item 2
	"   Charts         ",		// item 3
	"   Exit           ",		// item 4
};

// Menu items icon
static const uint8_t * const menu_items_icon[NUMBER_MENU_ITEMS] = {
	bitmap_menu_play,			// item 1
	bitmap_menu_settings,		// item 2
	bitmap_menu_charts,			// item 3
	bitmap_menu_exit,			// item 4
};

// Menu items axis Y
static const uint8_t menu_items_icon_axis_y[MENU_VISIBLE_ITEMS] = {
	2,							// icon frame 1
	24,							// icon frame 2
	46							// icon frame 3
};

// Screen and item location on the Menu
typedef struct {
	uint8_t screen;
	uint8_t location;
} screen_t;

// Scroll bar
typedef struct {
	uint8_t axis_x = 126;
	uint8_t axis_y = 0;
	uint8_t size_W = 3;
	uint8_t size_h = SCREEN_MENU_H / NUMBER_MENU_ITEMS;
} scr_menu_scroll_bar_t;

// Frames
typedef struct {
	uint8_t axis_x = 0;
	uint8_t axis_y = 0;
	uint8_t size_w = 123;
	uint8_t size_h = 20;
	uint8_t size_r = 3;
} scr_menu_frames_t;

static screen_t screen_menu;
static scr_menu_scroll_bar_t scroll_bar;
static scr_menu_frames_t frame_white;
static scr_menu_frames_t frame[MENU_VISIBLE_ITEMS];

/*****************************************************************************/
/* View - Menu game */
/*****************************************************************************/
static void view_scr_menu_game();

view_dynamic_t dyn_view_menu = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_menu_game
};

view_screen_t scr_menu_game = {
	&dyn_view_menu,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

static void view_scr_menu_game() {
#define AR_GAME_MENU_ICON_AXIS_X			(10)
#define AR_GAME_MENU_TEXT_AXIS_X			(32)

	// Scroll bar
	view_render.fillRect(	scroll_bar.axis_x - 1, \
							scroll_bar.axis_y, \
							scroll_bar.size_W, \
							scroll_bar.size_h, \
							WHITE);
	view_render.fillRect(	scroll_bar.axis_x, \
							0, \
							1, \
							SCREEN_MENU_H, \
							WHITE);

	for (uint8_t i = 0; i < MENU_VISIBLE_ITEMS; i++) {
		// Frames
		view_render.drawRoundRect(	frame[i].axis_x, \
									frame[i].axis_y, \
									frame[i].size_w, \
									frame[i].size_h, \
									frame[i].size_r, \
									WHITE);
		if (i == (screen_menu.location - screen_menu.screen)) {
			view_render.drawRoundRect(	frame_white.axis_x + 2, \
										frame_white.axis_y + 2, \
										frame_white.size_w - 5, \
										frame_white.size_h - 4, \
										frame_white.size_r, \
										WHITE);
			view_render.fillRect(	2, \
									frame_white.axis_y + 5, \
									3, \
									10, \
									WHITE);
			view_render.drawBitmap(	frame_white.axis_x + frame_white.size_w - 22, \
									menu_items_icon_axis_y[i] + 3, \
									bitmap_chose_icon, \
									MENU_CHOOSE_ICON_W, \
									MENU_CHOOSE_ICON_H, \
									WHITE);
		}
		// Icon
		view_render.drawBitmap(	AR_GAME_MENU_ICON_AXIS_X, \
								menu_items_icon_axis_y[i], \
								menu_items_icon[screen_menu.screen + i], \
								MENU_ICON_W, \
								MENU_ICON_H, \
								WHITE);
	}

	// Text Menu
	view_render.setTextSize(1);
	for (uint8_t i = 0; i < MENU_VISIBLE_ITEMS; i++) {
		view_render.setTextColor(WHITE);
		view_render.setCursor(AR_GAME_MENU_TEXT_AXIS_X, menu_items_icon_axis_y[i]+5);
		view_render.print(menu_items_name[screen_menu.screen + i]);
	}
}

/*****************************************************************************/
/* Handle - Menu game */
/*****************************************************************************/
static void update_menu_screen_choose() {
	// Frames location
	frame_white.axis_y =frame[screen_menu.location-screen_menu.screen].axis_y;
	frame[0].axis_y = 0;
	frame[1].axis_y = 22;
	frame[2].axis_y = 44;
	// update scroll bar
	scroll_bar.axis_y = (SCREEN_MENU_H*screen_menu.location / NUMBER_MENU_ITEMS);
}

static void screen_tran_menu() {
	switch (screen_menu.location) {
	case 0:	{ // item 1
		SCREEN_TRAN(scr_nobrake_game_handle,	&scr_nobrake_game	);
	} break;

	case 1:	{ // item 2
        SCREEN_TRAN(scr_game_settings_handle,	&scr_game_settings	);
	} break;

	case 2:	{ // item 3
        SCREEN_TRAN(scr_game_score_handle,		&scr_game_score		);
	} break;

	case 3: { // item 4
		scr_idle_set_return_screen(scr_menu_game_handle, &scr_menu_game);
		SCREEN_TRAN(scr_idle_handle,			&scr_idle			);
	} break;

	default:
		break;
	}
}

void scr_menu_game_handle(ak_msg_t* msg) {
	switch (msg->sig) {
	case SCREEN_ENTRY: {
		update_menu_screen_choose();
		// timer switch to scr_idle
		timer_set(	AC_TASK_DISPLAY_ID, \
					AC_DISPLAY_SHOW_IDLE, \
					AC_DISPLAY_IDLE_INTERVAL, \
					TIMER_ONE_SHOT);
	} break;

	case AC_DISPLAY_SHOW_IDLE: {
		timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE);
		scr_idle_set_return_screen(scr_menu_game_handle, &scr_menu_game);
		SCREEN_TRAN(scr_idle_handle,&scr_idle);
	} break;

	case AC_DISPLAY_BUTTON_MODE_PRESSED: {
		screen_tran_menu();
	} break;

	case AC_DISPLAY_BUTTON_UP_PRESSED: {
		if (screen_menu.location > 0) {
			screen_menu.location--;
		}

		if (frame_white.axis_y == frame[0].axis_y) {
			if (screen_menu.screen > 0) {
				screen_menu.screen--;
			}
		}
		else if (frame_white.axis_y == frame[1].axis_y) {
			frame_white.axis_y = frame[0].axis_y;
		}
		else if (frame_white.axis_y == frame[2].axis_y) {
			frame_white.axis_y = frame[1].axis_y;
		}

		update_menu_screen_choose();
	} break;

	case AC_DISPLAY_BUTTON_DOWN_PRESSED: {
		if (screen_menu.location < NUMBER_MENU_ITEMS-1) {
			screen_menu.location++;
		}

		if (frame_white.axis_y == frame[0].axis_y) {
			frame_white.axis_y = frame[1].axis_y;
		}
		else if (frame_white.axis_y == frame[1].axis_y) {
			frame_white.axis_y = frame[2].axis_y;
		}
		else if (frame_white.axis_y == frame[2].axis_y) {
			if (screen_menu.screen < NUMBER_MENU_ITEMS-MENU_VISIBLE_ITEMS) {
				screen_menu.screen++;
			}
		}

		update_menu_screen_choose();
	} break;

	default:
		break;
	}
}

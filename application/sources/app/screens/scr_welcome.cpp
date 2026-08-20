#include "scr_welcome.h"

static uint8_t welcome_frame;

static void welcome_draw_smoke(int16_t x, int16_t y, uint8_t frame) {
	int16_t drift = frame & 0x03;
	view_render.drawCircle(x - drift, y, 1, WHITE);
	view_render.drawCircle(x - 7 - drift, y - 3, 2, WHITE);
	view_render.drawPixel(x - 14 - drift, y - 7, WHITE);
}

static void welcome_draw_side_car(int16_t x, int16_t y, uint8_t frame) {
	y += (frame & 0x01) ? 1 : 0;
	view_render.drawBitmap(x, y, bitmap_car_side, 48, 20, WHITE);
	view_render.fillRect(x + 13, y + 4, 9, 5, BLACK);
	view_render.fillRect(x + 24, y + 3, 11, 6, BLACK);
	view_render.drawLine(x + 23, y + 4, x + 23, y + 15, BLACK);
	view_render.drawLine(x + 7, y + 12, x + 41, y + 12, BLACK);
	view_render.drawPixel(x + 45, y + 13, WHITE);
	view_render.fillCircle(x + 12, y + 17, 4, WHITE);
	view_render.fillCircle(x + 12, y + 17, 2, BLACK);
	view_render.fillCircle(x + 37, y + 17, 4, WHITE);
	view_render.fillCircle(x + 37, y + 17, 2, BLACK);
	welcome_draw_smoke(x + 8, y + 10, frame);
}

static void welcome_draw_road(uint8_t frame) {
	view_render.drawLine(8, 60, 120, 60, WHITE);
	for (uint8_t i = 0; i < 4; ++i) {
		int16_t y	  = 54 + (i * 2);
		int16_t half  = 4 + (i * 5);
		int16_t drift = (frame + i) & 0x01;
		view_render.drawLine(64 - half + drift, y, 64 + half + drift, y, WHITE);
	}
}

static void view_scr_welcome(void) {
	view_render.drawRoundRect(3, 3, 122, 58, 4, WHITE);
	view_render.setTextSize(2);
	view_render.setTextColor(WHITE);
	view_render.setCursor(16, 7);
	view_render.print("NO BRAKE");

	view_render.drawLine(15, 28, 113, 28, WHITE);
	welcome_draw_side_car(40, 32, welcome_frame);
	welcome_draw_road(welcome_frame);
}

view_dynamic_t dyn_view_welcome = {{.item_type = ITEM_TYPE_DYNAMIC}, view_scr_welcome};

view_screen_t scr_welcome = {
	&dyn_view_welcome,
	ITEM_NULL,
	ITEM_NULL,
	.focus_item = 0,
};

static void welcome_enter_menu(void) {
	timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_WELCOME_ANIM);
	BUZZER_PlayTones(tones_welcome_enter);
	SCREEN_TRAN(scr_menu_game_handle, &scr_menu_game);
}

void scr_welcome_handle(ak_msg_t *msg) {
	switch (msg->sig) {
	case SCREEN_ENTRY: {
		welcome_frame = 0;
		BUZZER_PlayTones(tones_welcome_start);
		timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_WELCOME_ANIM, AC_DISPLAY_WELCOME_ANIM_INTERVAL, TIMER_PERIODIC);
	} break;

	case AC_DISPLAY_WELCOME_ANIM: {
		welcome_frame++;
	} break;

	case AC_DISPLAY_BUTTON_MODE_PRESSED:
	case AC_DISPLAY_BUTTON_UP_PRESSED:
	case AC_DISPLAY_BUTTON_DOWN_PRESSED: {
		welcome_enter_menu();
	} break;

	case SCREEN_EXIT: {
		timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_WELCOME_ANIM);
	} break;

	default:
		break;
	}
}

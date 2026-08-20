#include "scr_game_settings.h"

static void draw_difficulty_card(int16_t x, uint8_t width, int16_t label_x,
                                 const char* label, uint8_t selected)
{
	if (selected)
	{
		view_render.fillRect(x, 24, width, 16, WHITE);
		view_render.setTextColor(BLACK);
	}
	else
	{
		view_render.drawRect(x, 24, width, 16, WHITE);
		view_render.setTextColor(WHITE);
	}

	view_render.setCursor(label_x, 29);
	view_render.print(label);
}

static void view_scr_game_settings()
{
	view_render.drawRoundRect(3, 3, 122, 58, 4, WHITE);
	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);

	view_render.setCursor(31, 7);
	view_render.print("SETTINGS");

	view_render.drawLine(10, 15, 118, 15, WHITE);

	uint8_t difficulty = nb_game_settings_get_difficulty();
	draw_difficulty_card(9, 30, 12, "EASY", difficulty == 0);
	draw_difficulty_card(47, 38, 48, "NORMAL", difficulty == 1);
	draw_difficulty_card(91, 29, 93, "HARD", difficulty == 2);
}

view_dynamic_t dyn_view_item_game_settings = {
    {.item_type = ITEM_TYPE_DYNAMIC},
    view_scr_game_settings};

view_screen_t scr_game_settings = {
    &dyn_view_item_game_settings,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};

void scr_game_settings_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case SCREEN_ENTRY:
	{
	}
	break;

	case AC_DISPLAY_BUTTON_MODE_PRESSED:
	{
		SCREEN_TRAN(scr_menu_game_handle, &scr_menu_game);
	}
	break;

	case AC_DISPLAY_BUTTON_UP_PRESSED:
	{
		nb_game_settings_change_difficulty(1);
	}
	break;

	case AC_DISPLAY_BUTTON_DOWN_PRESSED:
	{
		nb_game_settings_change_difficulty(-1);
	}
	break;

	default:
		break;
	}
}

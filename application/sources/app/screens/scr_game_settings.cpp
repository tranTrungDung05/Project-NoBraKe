#include "scr_game_settings.h"
#include "nb_game_track.h"

static const char* difficulty_name(uint8_t difficulty) {
    switch (difficulty) {
    case 0:
        return "EASY";
    case 1:
        return "NORMAL";
    default:
        return "HARD";
    }
}

static void view_scr_game_settings() {
    view_render.setTextSize(1);
    view_render.setTextColor(WHITE);

    view_render.setCursor(18, 2);
    view_render.print("SETTINGS");

    view_render.drawRect(0, 22, 128, 24, WHITE);

    view_render.setCursor(12, 31);
    view_render.print("DIFF:");
    view_render.setCursor(58, 31);
    view_render.print(difficulty_name(nb_game_settings_get_difficulty()));

    view_render.setCursor(8, 56);
    view_render.print("UP/DOWN  MODE:MENU");
}

view_dynamic_t dyn_view_item_game_settings = {
    { .item_type = ITEM_TYPE_DYNAMIC },
    view_scr_game_settings
};

view_screen_t scr_game_settings = {
    &dyn_view_item_game_settings,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};

void scr_game_settings_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case SCREEN_ENTRY: {
    } break;

    case AC_DISPLAY_BUTTON_MODE_PRESSED: {
        SCREEN_TRAN(scr_menu_game_handle, &scr_menu_game);
    } break;

    case AC_DISPLAY_BUTTON_UP_PRESSED: {
        nb_game_settings_change_difficulty(1);
    } break;

    case AC_DISPLAY_BUTTON_DOWN_PRESSED: {
        nb_game_settings_change_difficulty(-1);
    } break;

    default:
        break;
    }
}

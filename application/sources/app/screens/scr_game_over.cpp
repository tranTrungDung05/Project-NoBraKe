#include "scr_game_over.h"

static void view_scr_game_over(void) {
    view_render.drawRoundRect(3, 3, 122, 58, 4, WHITE);
    view_render.drawLine(12, 11, 34, 11, WHITE);
    view_render.drawLine(94, 11, 116, 11, WHITE);

    view_render.setTextSize(2);
    view_render.setTextColor(WHITE);
    view_render.setCursor(11, 16);
    view_render.print("GAME OVER");

    view_render.setTextSize(1);
    view_render.setCursor(32, 40);
    view_render.print("NO BRAKE ?");
}

view_dynamic_t dyn_view_item_game_over = {
    { .item_type = ITEM_TYPE_DYNAMIC },
    view_scr_game_over
};

view_screen_t scr_game_over = {
    &dyn_view_item_game_over,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};

void scr_game_over_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case AC_DISPLAY_BUTTON_MODE_PRESSED:
    case AC_DISPLAY_BUTTON_UP_PRESSED:
    case AC_DISPLAY_BUTTON_DOWN_PRESSED: {
        SCREEN_TRAN(scr_menu_game_handle, &scr_menu_game);
    } break;

    default:
        break;
    }
}

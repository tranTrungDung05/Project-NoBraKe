#include "scr_menu_game.h"

#define MENU_ITEM_COUNT   (4)
#define MENU_VISIBLE      (3)
#define MENU_ITEM_HEIGHT  (22)

static const char *menu_names[MENU_ITEM_COUNT] = {
    "Play",
    "Settings",
    "Charts",
    "Exit"
};

static const uint8_t *menu_icons[MENU_ITEM_COUNT] = {
    bitmap_menu_play,
    bitmap_menu_settings,
    bitmap_menu_charts,
    bitmap_menu_exit
};

static uint8_t first_item;
static uint8_t selected_item;

static void draw_menu_item(uint8_t slot, uint8_t item) {
    int16_t y = slot * MENU_ITEM_HEIGHT;
    uint8_t selected = item == selected_item;

    view_render.drawRoundRect(0, y, 123, 20, 3, WHITE);
    if (selected) {
        view_render.drawRoundRect(2, y + 2, 118, 16, 3, WHITE);
        view_render.fillRect(2, y + 5, 3, 10, WHITE);
        view_render.drawBitmap(99, y + 5, bitmap_chose_icon, 16, 10, WHITE);
    }

    view_render.drawBitmap(10, y + 2, menu_icons[item], 16, 16, WHITE);
    view_render.setCursor(32, y + 7);
    view_render.print(menu_names[item]);
}

static void view_scr_menu_game(void) {
    view_render.fillRect(125, 0, 2, 64, WHITE);
    view_render.fillRect(125, selected_item * 16, 3, 16, WHITE);

    view_render.setTextSize(1);
    view_render.setTextColor(WHITE);
    for (uint8_t slot = 0; slot < MENU_VISIBLE; ++slot) {
        draw_menu_item(slot, first_item + slot);
    }
}

view_dynamic_t dyn_view_menu = {
    { .item_type = ITEM_TYPE_DYNAMIC },
    view_scr_menu_game
};

view_screen_t scr_menu_game = {
    &dyn_view_menu,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};

static void move_selection(int8_t direction) {
    int16_t next = (int16_t)selected_item + direction;
    if (next < 0 || next >= MENU_ITEM_COUNT) {
        return;
    }

    selected_item = (uint8_t)next;
    if (selected_item < first_item) {
        first_item = selected_item;
    }
    if (selected_item >= first_item + MENU_VISIBLE) {
        first_item = selected_item - MENU_VISIBLE + 1;
    }
}

static void open_selected_item(void) {
    switch (selected_item) {
    case 0:
        SCREEN_TRAN(scr_nobrake_game_handle, &scr_nobrake_game);
        break;
    case 1:
        SCREEN_TRAN(scr_game_settings_handle, &scr_game_settings);
        break;
    case 2:
        SCREEN_TRAN(scr_game_score_handle, &scr_game_score);
        break;
    case 3:
        scr_idle_set_return_screen(scr_menu_game_handle, &scr_menu_game);
        SCREEN_TRAN(scr_idle_handle, &scr_idle);
        break;
    default:
        break;
    }
}

void scr_menu_game_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case SCREEN_ENTRY:
        timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE,
                  AC_DISPLAY_IDLE_INTERVAL, TIMER_ONE_SHOT);
        break;

    case AC_DISPLAY_SHOW_IDLE:
        timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE);
        scr_idle_set_return_screen(scr_menu_game_handle, &scr_menu_game);
        SCREEN_TRAN(scr_idle_handle, &scr_idle);
        break;

    case AC_DISPLAY_BUTTON_MODE_PRESSED:
        open_selected_item();
        break;

    case AC_DISPLAY_BUTTON_UP_PRESSED:
        move_selection(-1);
        break;

    case AC_DISPLAY_BUTTON_DOWN_PRESSED:
        move_selection(1);
        break;

    default:
        break;
    }
}

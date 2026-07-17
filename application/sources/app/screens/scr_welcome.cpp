#include "scr_welcome.h"
#include "screens.h"
#include "buzzer.h"

static uint8_t welcome_frame;

static const Tone_TypeDef tones_welcome_start[] = {
    {900, 2},
    {0, 1},
    {1400, 2},
    {0, 1},
    {2200, 3},
    {0, 0}
};

static const Tone_TypeDef tones_welcome_enter[] = {
    {1800, 2},
    {0, 1},
    {2600, 2},
    {0, 0}
};

static void welcome_draw_smoke(int16_t x, int16_t y, uint8_t frame) {
    int16_t drift = frame & 0x03;

    view_render.drawCircle(x - drift, y - 1, 1, WHITE);
    view_render.drawCircle(x - 7 - drift, y - 4, 2, WHITE);
    if ((frame & 0x01) == 0) {
        view_render.drawPixel(x - 13 - drift, y - 7, WHITE);
        view_render.drawCircle(x - 16 - drift, y - 9, 1, WHITE);
    }
    else {
        view_render.drawCircle(x - 13 - drift, y - 8, 1, WHITE);
        view_render.drawPixel(x - 18 - drift, y - 10, WHITE);
    }
}

static void welcome_draw_big_car(int16_t x, int16_t y, uint8_t frame) {
    int16_t shake = (frame & 0x01) ? 1 : -1;

    y += shake;
    view_render.drawLine(x + 16, y + 3, x + 26, y - 5, WHITE);
    view_render.drawLine(x + 26, y - 5, x + 45, y - 5, WHITE);
    view_render.drawLine(x + 45, y - 5, x + 56, y + 3, WHITE);
    view_render.fillRect(x + 8, y + 4, 58, 13, WHITE);
    view_render.fillRect(x + 23, y - 1, 8, 5, BLACK);
    view_render.fillRect(x + 35, y - 1, 9, 5, BLACK);
    view_render.fillRect(x + 13, y + 8, 6, 3, BLACK);
    view_render.fillRect(x + 55, y + 8, 6, 3, BLACK);
    view_render.fillCircle(x + 22, y + 17, 6, WHITE);
    view_render.fillCircle(x + 52, y + 17, 6, WHITE);
    view_render.fillCircle(x + 22, y + 17, 2, BLACK);
    view_render.fillCircle(x + 52, y + 17, 2, BLACK);
    welcome_draw_smoke(x + 8, y + 10, frame);
}

static void view_scr_welcome(void) {
    view_render.setTextSize(2);
    view_render.setTextColor(WHITE);
    view_render.setCursor(22, 3);
    view_render.print("NoBraK4");

    welcome_draw_big_car(28, 31, welcome_frame);

    view_render.setTextSize(1);
    view_render.setCursor(20, 55);
    view_render.print("PRESS ANY BUTTON");
}

view_dynamic_t dyn_view_welcome = {
    { .item_type = ITEM_TYPE_DYNAMIC },
    view_scr_welcome
};

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

void scr_welcome_handle(ak_msg_t* msg) {
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

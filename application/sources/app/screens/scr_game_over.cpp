#include "scr_game_over.h"
#include "screens.h"
#include "timer.h"

#define GAME_OVER_ANIM_INTERVAL (120)

static uint8_t over_frame;

static void draw_wreck(void) {
    uint8_t smoke = over_frame & 0x07;

    view_render.drawCircle(42 - smoke, 39 - smoke, 1, WHITE);
    view_render.drawCircle(35 - smoke, 35 - smoke, 2, WHITE);
    view_render.drawPixel(29 - smoke, 31 - smoke, WHITE);
    view_render.drawCircle(86 + smoke, 39 - smoke, 1, WHITE);
    view_render.drawCircle(93 + smoke, 35 - smoke, 2, WHITE);
    view_render.drawPixel(99 + smoke, 31 - smoke, WHITE);

    view_render.drawLine(49, 45, 79, 45, WHITE);
    view_render.drawLine(53, 39, 75, 39, WHITE);
    view_render.drawLine(49, 45, 53, 39, WHITE);
    view_render.drawLine(79, 45, 75, 39, WHITE);
    view_render.drawLine(58, 45, 61, 39, WHITE);
    view_render.drawLine(67, 45, 70, 39, WHITE);

    view_render.drawCircle(53, 36, 3, WHITE);
    view_render.drawCircle(61, 35, 3, WHITE);
    view_render.drawCircle(69, 35, 3, WHITE);
    view_render.drawCircle(77, 36, 3, WHITE);

    if ((over_frame & 0x01) == 0) {
        view_render.drawPixel(53, 36, WHITE);
        view_render.drawPixel(61, 35, WHITE);
        view_render.drawPixel(69, 35, WHITE);
        view_render.drawPixel(77, 36, WHITE);
    }
}

static void view_scr_game_over(void) {
    view_render.drawMoon(104, 7, 9, WHITE);
    view_render.drawPixel(10, 9, WHITE);
    view_render.drawPixel(30, 5, WHITE);
    view_render.drawPixel(78, 12, WHITE);
    view_render.drawPixel(118, 27, WHITE);
    view_render.drawPixel(52, 4, WHITE);
    view_render.drawPixel(91, 6, WHITE);

    view_render.setTextSize(2);
    view_render.setTextColor(WHITE);
    view_render.setCursor(11, 5);
    view_render.print("GAME OVER");

    view_render.drawLine(18, 62, 48, 34, WHITE);
    view_render.drawLine(110, 62, 80, 34, WHITE);
    view_render.drawLine(54, 61, 62, 50, WHITE);
    view_render.drawLine(74, 61, 67, 50, WHITE);

    view_render.setTextSize(1);
    view_render.setCursor(34, 23);
    view_render.print("NO BRAKE?");

    draw_wreck();

    view_render.setCursor(7, 42);
    view_render.print("BOO");
    view_render.setCursor(100, 42);
    view_render.print("BOO");
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
    case SCREEN_ENTRY: {
        over_frame = 0;
        timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_RENDER_SCREEN, GAME_OVER_ANIM_INTERVAL, TIMER_PERIODIC);
    } break;

    case AC_DISPLAY_RENDER_SCREEN: {
        over_frame++;
    } break;

    case AC_DISPLAY_BUTTON_MODE_PRESSED:
    case AC_DISPLAY_BUTTON_UP_PRESSED:
    case AC_DISPLAY_BUTTON_DOWN_PRESSED: {
        timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_RENDER_SCREEN);
        SCREEN_TRAN(scr_menu_game_handle, &scr_menu_game);
    } break;

    case SCREEN_EXIT: {
        timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_RENDER_SCREEN);
    } break;

    default:
        break;
    }
}

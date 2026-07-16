#include "scr_game_finish.h"
#include "app.h"
#include "screens.h"
#include "timer.h"

#define FINISH_ANIM_INTERVAL (120)

static uint8_t finish_frame;

static void finish_draw_checkers(int16_t x, int16_t y, uint8_t cols, uint8_t rows) {
    for (uint8_t row = 0; row < rows; ++row) {
        for (uint8_t col = 0; col < cols; ++col) {
            if (((row + col) & 0x01) == 0) {
                view_render.fillRect(x + (col * 4), y + (row * 3), 4, 3, WHITE);
            }
        }
    }
}

static void finish_draw_clap(int16_t x, int16_t y, uint8_t frame) {
    int8_t open = (frame & 0x01) ? 5 : 0;

    view_render.fillCircle(x - 4 - open, y, 3, WHITE);
    view_render.fillCircle(x + 4 + open, y, 3, WHITE);
    view_render.drawLine(x - 8 - open, y + 4, x - 15, y + 11, WHITE);
    view_render.drawLine(x + 8 + open, y + 4, x + 15, y + 11, WHITE);
    if (open == 0) {
        view_render.drawPixel(x, y - 4, WHITE);
        view_render.drawPixel(x, y + 4, WHITE);
    }
}

static void finish_draw_party_popper(int16_t x, int16_t y, int8_t dir, uint8_t frame) {
    view_render.drawTriangle(x, y, x + (dir * 10), y - 4, x + (dir * 4), y + 7, WHITE);
    view_render.drawLine(x + (dir * 3), y - 3, x + (dir * (13 + (frame & 0x03))), y - 13, WHITE);
    view_render.drawLine(x + (dir * 5), y, x + (dir * 17), y - 6 - (frame & 0x01), WHITE);
    view_render.drawLine(x + (dir * 5), y + 3, x + (dir * 14), y + 2, WHITE);
}

static void finish_draw_confetti(uint8_t frame) {
    static const uint8_t dots[][3] = {
        {6, 2, 1}, {17, 18, 2}, {28, 7, 3}, {39, 28, 1},
        {51, 12, 2}, {64, 0, 3}, {77, 22, 1}, {89, 9, 2},
        {101, 31, 3}, {113, 15, 1}, {123, 4, 2}
    };

    for (uint8_t i = 0; i < sizeof(dots) / sizeof(dots[0]); ++i) {
        int16_t y = (int16_t)((dots[i][1] + (frame * dots[i][2])) & 0x3F);
        if (y < 53) {
            view_render.drawPixel(dots[i][0], y, WHITE);
            if (((frame + i) & 0x01) == 0) {
                view_render.drawPixel(dots[i][0] + 1, y + 1, WHITE);
            }
            else {
                view_render.drawLine(dots[i][0] - 1, y, dots[i][0] + 1, y, WHITE);
            }
        }
    }

    finish_draw_party_popper(7, 58, 1, frame);
    finish_draw_party_popper(121, 58, -1, frame + 1);
}

static void view_scr_game_finish(void) {
    finish_draw_confetti(finish_frame);
    finish_draw_checkers(12, 4, 6, 3);
    finish_draw_checkers(92, 4, 6, 3);

    view_render.setTextSize(2);
    view_render.setTextColor(WHITE);
    view_render.setCursor(22, 17);
    view_render.print("FINISH");

    view_render.setTextSize(1);
    view_render.setCursor(34, 36);
    view_render.print("YOU MADE IT!");

    finish_draw_clap(21, 46, finish_frame);
    finish_draw_clap(107, 46, finish_frame + 1);
}

view_dynamic_t dyn_view_item_game_finish = {
    { .item_type = ITEM_TYPE_DYNAMIC },
    view_scr_game_finish
};

view_screen_t scr_game_finish = {
    &dyn_view_item_game_finish,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};

void scr_game_finish_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case SCREEN_ENTRY: {
        finish_frame = 0;
        timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_RENDER_SCREEN, FINISH_ANIM_INTERVAL, TIMER_PERIODIC);
    } break;

    case AC_DISPLAY_RENDER_SCREEN: {
        finish_frame++;
    } break;

    case AC_DISPLAY_BUTTON_MODE_PRESSED: {
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

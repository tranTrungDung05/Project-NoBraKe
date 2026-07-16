#include "scr_game_score.h"

static uint16_t best_times[NB_GAME_SCORE_COUNT];

void nb_game_score_record_finish(uint16_t elapsed_s) {
    if (elapsed_s == 0) {
        return;
    }

    for (uint8_t i = 0; i < NB_GAME_SCORE_COUNT; ++i) {
        if (best_times[i] != 0 && elapsed_s >= best_times[i]) {
            continue;
        }

        for (uint8_t j = NB_GAME_SCORE_COUNT - 1; j > i; --j) {
            best_times[j] = best_times[j - 1];
        }
        best_times[i] = elapsed_s;
        break;
    }
}

static void view_scr_game_score() {
    view_render.setTextSize(1);
    view_render.setTextColor(WHITE);
    view_render.setCursor(43, 2);
    view_render.print("CHARTS");

    view_render.drawRect(0, 14, 128, 38, WHITE);

    for (uint8_t i = 0; i < NB_GAME_SCORE_COUNT; ++i) {
        view_render.setCursor(16, 21 + (i * 10));
        view_render.print(i + 1);
        view_render.print(".");
        view_render.setCursor(44, 21 + (i * 10));
        if (best_times[i] == 0) {
            view_render.print("--");
        }
        else {
            view_render.print(best_times[i]);
            view_render.print("s");
        }
    }

    view_render.drawLine(28, 56, 100, 56, WHITE);
}

view_dynamic_t dyn_view_item_game_score = {
    { .item_type = ITEM_TYPE_DYNAMIC },
    view_scr_game_score
};

view_screen_t scr_game_score = {
    &dyn_view_item_game_score,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};

void scr_game_score_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case SCREEN_ENTRY: {
        APP_DBG_SIG("SCREEN_ENTRY: Charts\n");
    } break;

    case AC_DISPLAY_BUTTON_MODE_PRESSED:
    case AC_DISPLAY_BUTTON_DOWN_PRESSED: {
        SCREEN_TRAN(scr_menu_game_handle, &scr_menu_game);
    } break;

    default:
        break;
    }
}

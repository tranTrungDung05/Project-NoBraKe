#include "scr_nobrake_game.h"

uint8_t game_state;
static int8_t steer_hold;
static uint8_t throttle_hold;
static uint8_t throttle_sound_wait;
static uint8_t finish_frame;

static void draw_sky(void) {
    uint8_t drift = (uint8_t)((track.pos / 40) % 152);
    int16_t first_cloud_x = 12 - drift;
    int16_t second_cloud_x = 70 - drift;

    if (first_cloud_x < -14) {
        first_cloud_x += 152;
    }
    if (second_cloud_x < -14) {
        second_cloud_x += 152;
    }

    view_render.drawBitmap(first_cloud_x, 11, bitmap_cloud, 24, 8, WHITE);
    view_render.drawBitmap(second_cloud_x, 15, bitmap_cloud, 24, 8, WHITE);
}

/* Render helpers: road surface, rumble strips, and lane markers */
static void draw_track(void) {
    uint8_t stripe_base = (uint8_t)(track.pos / NB_GAME_SEGMENT_LENGTH);

    for (uint8_t i = 1; i < NB_GAME_VISIBLE_LINES; ++i) {
        nb_game_line_t *near_line = &view_lines[i];
        nb_game_line_t *far_line = &view_lines[i - 1];
        if (!near_line->visible || near_line->Y <= 10) {
            continue;
        }

        int16_t far_left = far_line->X - far_line->W;
        int16_t far_right = far_line->X + far_line->W;
        int16_t near_left = near_line->X - near_line->W;
        int16_t near_right = near_line->X + near_line->W;

        view_render.drawLine(far_left, far_line->Y,
                             near_left, near_line->Y, WHITE);
        view_render.drawLine(far_right, far_line->Y,
                             near_right, near_line->Y, WHITE);

        if ((stripe_base + i) % 2 != 0) {
            view_render.drawLine(far_left - 3, far_line->Y,
                                 near_left - 3, near_line->Y, WHITE);
            view_render.drawLine(far_right + 3, far_line->Y,
                                 near_right + 3, near_line->Y, WHITE);
        }

        if ((stripe_base + i) % 3 == 0) {
            for (uint8_t lane = 1; lane < NB_GAME_LANE_COUNT; ++lane) {
                int16_t far_x = far_left + ((far_line->W * 2 * lane) / NB_GAME_LANE_COUNT);
                int16_t near_x = near_left + ((near_line->W * 2 * lane) / NB_GAME_LANE_COUNT);
                view_render.drawLine(far_x, far_line->Y, near_x, near_line->Y, WHITE);
            }
        }
    }
}

static void draw_trees(void) {
    for (uint8_t i = 0; i < NB_GAME_TREE_COUNT; ++i) {
        const nb_game_tree_t *tree = &view_trees[i];
        if (tree->view_index == 255 || tree->size == 0) {
            continue;
        }

        if (tree->size >= 20) {
            view_render.drawBitmap(tree->x - 8, tree->y - 24, bitmap_tree_large, 16, 24, WHITE);
        }
        else {
            int16_t half = tree->size / 2;
            view_render.drawLine(tree->x, tree->y, tree->x, tree->y - tree->size / 3, WHITE);
            view_render.drawTriangle(tree->x, tree->y - tree->size,
                                     tree->x - half, tree->y - tree->size / 3,
                                     tree->x + half, tree->y - tree->size / 3, WHITE);
        }
    }
}

static const unsigned char *get_obstacle_bitmap(uint8_t type) {
    switch (type) {
    case NB_GAME_OBSTACLE_BARRIER:
        return bitmap_obstacle_barrier;

    case NB_GAME_OBSTACLE_CAR:
        return bitmap_obstacle_car;

    case NB_GAME_OBSTACLE_STONE:
    default:
        return bitmap_obstacle_stone;
    }
}

static void draw_traffic(void) {
    for (uint8_t i = 0; i < nb_game_obstacle_count(); ++i) {
        const nb_game_obstacle_t *obstacle = nb_game_obstacle_at(i);
        if (obstacle == 0 || obstacle->view_index == 255 || obstacle->y <= 18) {
            continue;
        }

        if (obstacle->view_index >= NB_GAME_VISIBLE_LINES) {
            continue;
        }

        const unsigned char *bitmap = get_obstacle_bitmap(obstacle->type);
        int16_t x = obstacle->x - (NB_GAME_OBSTACLE_W / 2);
        int16_t y = obstacle->y - NB_GAME_OBSTACLE_H;

        view_render.drawBitmap(x, y, bitmap, NB_GAME_OBSTACLE_W, NB_GAME_OBSTACLE_H, WHITE);
    }
}

/* Render helpers: finish gate animation */
static void draw_finish_gate(void) {
    int16_t y = 16 + ((int16_t)finish_frame * 2);
    if (y > 50) {
        y = 50;
    }

    view_render.drawLine(20, y, 20, 63, WHITE);
    view_render.drawLine(108, y, 108, 63, WHITE);
    view_render.drawRect(24, y, 80, 12, WHITE);
    for (uint8_t col = 0; col < 10; ++col) {
        for (uint8_t row = 0; row < 3; ++row) {
            if ((col + row + finish_frame) % 2 == 0) {
                view_render.fillRect(25 + (col * 8), y + 1 + (row * 3), 8, 3, WHITE);
            }
        }
    }

    if (y < 36) {
        view_render.setTextSize(1);
        view_render.setTextColor(BLACK);
        view_render.setCursor(45, y + 3);
        view_render.print("FINISH");
    }
}

/* Render helpers: HUD values */
static void draw_hud(void) {
    view_render.setTextSize(1);
    view_render.setTextColor(WHITE);
    view_render.drawCircle(4, 4, 3, WHITE);
    view_render.drawLine(4, 4, 4, 2, WHITE);
    view_render.drawLine(4, 4, 6, 5, WHITE);
    view_render.setCursor(10, 0);
    view_render.print(nb_game_track_get_time_left_s());
    view_render.print("s");

    view_render.setCursor(34, 0);
    view_render.print(car.speed);
    view_render.print(" km/h");
}

/*****************************************************************************/
/* View - Render */
/*****************************************************************************/
/* Main dynamic screen renderer */
static void draw_game() {
    if (game_state != GAME_PLAY && game_state != GAME_FINISHING) {
        return;
    }

    draw_sky();
    draw_track();
    draw_trees();
    draw_traffic();

    if (game_state == GAME_FINISHING) {
        draw_finish_gate();
        view_render.drawBitmap(car.x, car.y, bitmap_car_straight,
                               NB_GAME_CAR_W, NB_GAME_CAR_H, WHITE);
    }
    else if (car.visible) {
        view_render.drawBitmap(car.x, car.y, bitmap_car_straight,
                               NB_GAME_CAR_W, NB_GAME_CAR_H, WHITE);
        if (car.is_crash) {
            view_render.drawRect(car.x - 1, car.y - 1,
                                 NB_GAME_CAR_W + 2, NB_GAME_CAR_H + 2, WHITE);
        }
    }

    draw_hud();
}

view_dynamic_t dyn_view_item_nobrake_game = {
    { .item_type = ITEM_TYPE_DYNAMIC },
    draw_game
};

view_screen_t scr_nobrake_game = {
    &dyn_view_item_nobrake_game,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};

/*****************************************************************************/
/* Handle - Logic */
/*****************************************************************************/
/* Main screen state machine and input routing */
void scr_nobrake_game_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case SCREEN_ENTRY: {
        task_post_pure_msg(NB_GAME_CAR_ID, NB_GAME_CAR_SETUP);
        task_post_pure_msg(NB_GAME_TRACK_ID, NB_GAME_TRACK_SETUP);
        task_post_pure_msg(NB_GAME_OBSTACLE_ID, NB_GAME_OBSTACLE_SETUP);
        steer_hold = 0;
        throttle_hold = 0;
        throttle_sound_wait = 0;
        finish_frame = 0;

        game_state = GAME_PLAY;
        timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE);

        timer_set(AC_TASK_DISPLAY_ID, NB_GAME_TIME_TICK, NB_GAME_TIME_TICK_INTERVAL, TIMER_PERIODIC);
    } break;

    case NB_GAME_TIME_TICK: {
        if (game_state == GAME_FINISHING) {
            if (car.speed == 0) {
                game_state = GAME_WIN;
                timer_remove_attr(AC_TASK_DISPLAY_ID, NB_GAME_TIME_TICK);
                SCREEN_TRAN(scr_game_finish_handle, &scr_game_finish);
                break;
            }

            if (finish_frame < 24) {
                finish_frame++;
            }
            task_post_pure_msg(NB_GAME_CAR_ID, NB_GAME_CAR_FINISH_BRAKE);
            break;
        }

        if (game_state != GAME_PLAY) {
            break;
        }

        if (car.is_crash ||
            (nb_game_track_get_time_left_s() == 0 && track.pos < track.length)) {
            game_state = GAME_OVER;
            BUZZER_PlayTones(tones_game_crash);
            timer_remove_attr(AC_TASK_DISPLAY_ID, NB_GAME_TIME_TICK);
            steer_hold = 0;
            SCREEN_TRAN(scr_game_over_handle, &scr_game_over);
            break;
        }

        if (track.pos >= track.length) {
            game_state = GAME_FINISHING;
            finish_frame = 0;
            throttle_hold = 0;
            nb_game_score_record_finish(nb_game_track_get_elapsed_s());
            BUZZER_PlayTones(tones_game_finish);
            steer_hold = 0;
            task_post_pure_msg(NB_GAME_CAR_ID, NB_GAME_CAR_THROTTLE_RELEASE);
            break;
        }

        if (throttle_sound_wait > 0) {
            throttle_sound_wait--;
        }

        if (car.state) {
            if (steer_hold < 0) {
                task_post_pure_msg(NB_GAME_CAR_ID, NB_GAME_CAR_TURN_LEFT);
            }
            else if (steer_hold > 0) {
                task_post_pure_msg(NB_GAME_CAR_ID, NB_GAME_CAR_TURN_RIGHT);
            }
            task_post_pure_msg(NB_GAME_CAR_ID, NB_GAME_CAR_UPDATE);
        }
        else {
            task_post_pure_msg(NB_GAME_TRACK_ID, NB_GAME_TRACK_UPDATE);
        }
    } break;

    case AC_DISPLAY_BUTTON_UP_PRESSED:
    case AC_DISPLAY_BUTTON_UP_LONG_PRESSED: {
        if (game_state == GAME_PLAY) {
            steer_hold = 1;
        }
    } break;

    case AC_DISPLAY_BUTTON_DOWN_PRESSED:
    case AC_DISPLAY_BUTTON_DOWN_LONG_PRESSED: {
        if (game_state == GAME_PLAY) {
            steer_hold = -1;
        }
    } break;

    case AC_DISPLAY_BUTTON_UP_RELEASED:
    case AC_DISPLAY_BUTTON_DOWN_RELEASED: {
        if (game_state == GAME_PLAY) {
            steer_hold = 0;
        }
    } break;

    case AC_DISPLAY_BUTTON_MODE_PRESSED: {
        if (game_state == GAME_PLAY) {
            if (throttle_hold == 0) {
                throttle_hold = 1;
                if (throttle_sound_wait == 0) {
                    BUZZER_PlayTones(tones_game_throttle);
                    throttle_sound_wait = 3;
                }
                task_post_pure_msg(NB_GAME_CAR_ID, NB_GAME_CAR_THROTTLE);
            }
        }
    } break;

    case AC_DISPLAY_BUTTON_MODE_RELEASED: {
        if (throttle_hold != 0) {
            throttle_hold = 0;
            task_post_pure_msg(NB_GAME_CAR_ID, NB_GAME_CAR_THROTTLE_RELEASE);
        }
    } break;

    case SCREEN_EXIT: {
        timer_remove_attr(AC_TASK_DISPLAY_ID, NB_GAME_TIME_TICK);
        steer_hold = 0;
        task_post_pure_msg(NB_GAME_CAR_ID, NB_GAME_CAR_RESET);
        task_post_pure_msg(NB_GAME_OBSTACLE_ID, NB_GAME_OBSTACLE_RESET);
    } break;

    default:
        break;
    }
}

#include "scr_nobrake_game.h"
#include "nb_game_track.h"
#include "nb_game_car.h"
#include "nb_game_obstacle.h"
#include "scr_game_score.h"
#include "screens_bitmap.h"
#include "buzzer.h"

uint8_t game_state;
static int8_t steer_hold;
static uint8_t throttle_hold;
static uint8_t throttle_sound_wait;
static uint8_t finish_frame;

static const Tone_TypeDef tones_game_throttle[] = {
    {1200, 1},
    {1800, 1},
    {0, 0}
};

static const Tone_TypeDef tones_game_crash[] = {
    {240, 5},
    {0, 2},
    {160, 8},
    {0, 0}
};

static const Tone_TypeDef tones_game_finish[] = {
    {1600, 3},
    {0, 1},
    {2200, 3},
    {0, 1},
    {3200, 5},
    {0, 0}
};

static void post_steer(int8_t dir) {
    if (dir < 0) {
        task_post_pure_msg(NB_GAME_CAR_ID, NB_GAME_CAR_ACTION_LEFT);
    }
    else if (dir > 0) {
        task_post_pure_msg(NB_GAME_CAR_ID, NB_GAME_CAR_ACTION_RIGHT);
    }
}

static void start_steer_repeat(int8_t dir) {
    if (steer_hold == dir) {
        post_steer(dir);
        return;
    }

    steer_hold = dir;
    post_steer(steer_hold);
    timer_remove_attr(AC_TASK_DISPLAY_ID, NB_GAME_MOVE_LEFT_REPEAT);
    timer_remove_attr(AC_TASK_DISPLAY_ID, NB_GAME_MOVE_RIGHT_REPEAT);
    timer_set(AC_TASK_DISPLAY_ID,
              dir < 0 ? NB_GAME_MOVE_LEFT_REPEAT : NB_GAME_MOVE_RIGHT_REPEAT,
              NB_GAME_LONG_PRESS_MOVE_INTERVAL,
              TIMER_ONE_SHOT);
}

static void stop_steer_repeat(void) {
    steer_hold = 0;
    timer_remove_attr(AC_TASK_DISPLAY_ID, NB_GAME_MOVE_LEFT_REPEAT);
    timer_remove_attr(AC_TASK_DISPLAY_ID, NB_GAME_MOVE_RIGHT_REPEAT);
    task_post_pure_msg(NB_GAME_CAR_ID, NB_GAME_CAR_ACTION_STRAIGHT);
}

static void repeat_steer(int8_t dir) {
    if (game_state != GAME_PLAY || steer_hold != dir) {
        return;
    }

    post_steer(dir);
    timer_set(AC_TASK_DISPLAY_ID,
              dir < 0 ? NB_GAME_MOVE_LEFT_REPEAT : NB_GAME_MOVE_RIGHT_REPEAT,
              NB_GAME_LONG_PRESS_MOVE_INTERVAL,
              TIMER_ONE_SHOT);
}

static void draw_sky(void) {
    view_render.drawMoon(98, 12, 14, WHITE);
    view_render.drawPixel(7, 13, WHITE);
    view_render.drawPixel(18, 24, WHITE);
    view_render.drawPixel(31, 11, WHITE);
    view_render.drawPixel(48, 18, WHITE);
    view_render.drawPixel(69, 9, WHITE);
    view_render.drawPixel(83, 24, WHITE);
    view_render.drawPixel(119, 31, WHITE);
}

static void draw_track(void) {
    uint8_t stripe_base = (uint8_t)(track.pos / NB_GAME_SEGMENT_LENGTH);

    for (uint8_t i = 1; i < NB_GAME_VISIBLE_LINES; ++i) {
        nb_game_line_t *near_line = &view_lines[i];
        nb_game_line_t *far_line = &view_lines[i - 1];
        if (!near_line->visible || near_line->Y <= 10) {
            continue;
        }

        int16_t far_rumble = far_line->W + (far_line->W / 5);
        int16_t near_rumble = near_line->W + (near_line->W / 5);
        uint8_t stripe = (uint8_t)((stripe_base + i) & 0x01);

        if (stripe) {
            view_render.fillTriangle(far_line->X - far_rumble, far_line->Y,
                                     far_line->X - far_line->W, far_line->Y,
                                     near_line->X - near_rumble, near_line->Y, WHITE);
            view_render.fillTriangle(far_line->X - far_line->W, far_line->Y,
                                     near_line->X - near_rumble, near_line->Y,
                                     near_line->X - near_line->W, near_line->Y, WHITE);

            view_render.fillTriangle(far_line->X + far_line->W, far_line->Y,
                                     far_line->X + far_rumble, far_line->Y,
                                     near_line->X + near_line->W, near_line->Y, WHITE);
            view_render.fillTriangle(far_line->X + far_rumble, far_line->Y,
                                     near_line->X + near_line->W, near_line->Y,
                                     near_line->X + near_rumble, near_line->Y, WHITE);
        }

        view_render.drawLine(far_line->X - far_line->W, far_line->Y,
                             near_line->X - near_line->W, near_line->Y, WHITE);
        view_render.drawLine(far_line->X + far_line->W, far_line->Y,
                             near_line->X + near_line->W, near_line->Y, WHITE);

        if (((stripe_base + i) % 3) == 0) {
            for (uint8_t lane = 1; lane < NB_GAME_LANE_COUNT; ++lane) {
                int16_t far_x = far_line->X - far_line->W + (((far_line->W * 2) * lane) / NB_GAME_LANE_COUNT);
                int16_t near_x = near_line->X - near_line->W + (((near_line->W * 2) * lane) / NB_GAME_LANE_COUNT);
                view_render.drawLine(far_x, far_line->Y, near_x, near_line->Y, WHITE);
            }
        }
    }
}

static void draw_tree(int16_t x, int16_t y, int16_t size) {
    view_render.drawLine(x, y, x, y + size, WHITE);
    view_render.drawTriangle(x, y - size, x - size, y + 1, x + size, y + 1, WHITE);
}

static void draw_trees(void) {
    uint8_t segment_base = (uint8_t)(track.pos / NB_GAME_SEGMENT_LENGTH);

    for (uint8_t i = 3; i < NB_GAME_VISIBLE_LINES; ++i) {
        if (((segment_base + i) % 11) != 0) {
            continue;
        }

        nb_game_line_t *line = &view_lines[i];
        if (!line->visible || line->Y <= 18) {
            continue;
        }

        int16_t size = line->W / 9;
        if (size < 2) {
            size = 2;
        }
        if (size > 6) {
            size = 6;
        }

        int16_t side = ((segment_base + i) & 0x02) ? -1 : 1;
        int16_t x = line->X + (side * (line->W + (line->W / 2)));
        int16_t y = line->Y - 2;
        if (x > -8 && x < LCD_WIDTH + 8) {
            draw_tree(x, y, size);
        }
    }
}

static void draw_obstacles(void) {
    for (uint8_t i = 0; i < NB_GAME_OBSTACLE_COUNT; ++i) {
        const nb_game_obstacle_t *obstacle = &nb_game_obstacles[i];
        uint8_t view_index = nb_game_obstacle_get_view_index(obstacle);
        if (view_index == 0xFF || !view_lines[view_index].visible) {
            continue;
        }

        nb_game_line_t *line = &view_lines[view_index];
        if (line->Y <= 10) {
            continue;
        }

        int16_t lane_width = (line->W * 2) / NB_GAME_LANE_COUNT;
        int16_t left = line->X - line->W;
        for (uint8_t lane = 0; lane < NB_GAME_LANE_COUNT; ++lane) {
            if ((obstacle->lane_mask & (1U << lane)) == 0U) {
                continue;
            }

            int16_t x = left + (lane_width * lane) + (lane_width / 2) - 8;
            int16_t y = line->Y - 5;
            view_render.drawBitmap(x, y, bitmap_obstacle, 16, 10, WHITE);
        }
    }
}

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
            if (((col + row + finish_frame) & 0x01) == 0) {
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

static void draw_hud(void) {
    view_render.setTextSize(1);
    view_render.setTextColor(WHITE);
    view_render.setCursor(0, 0);
    view_render.print(nb_game_track_get_progress_percent());
    view_render.print("%");
    view_render.setCursor(39, 0);
    view_render.print(car.speed);
    view_render.print("kmh");
    view_render.setCursor(96, 0);
    view_render.print(nb_game_track_get_time_left_s());
    view_render.print("s");
}

/*****************************************************************************/
/* View - Render */
/*****************************************************************************/
static void draw_game() {
    if (game_state == GAME_PLAY) {
        draw_sky();
        draw_track();
        draw_trees();
        draw_obstacles();

        if (car.visible) {
            const unsigned char *sprite = bitmap_car_straight;
            if (car.action_image == NB_GAME_CAR_ACTION_IMAGE_LEFT) {
                sprite = bitmap_car_left;
            }
            else if (car.action_image == NB_GAME_CAR_ACTION_IMAGE_RIGHT) {
                sprite = bitmap_car_right;
            }
            view_render.drawBitmap(car.x, car.y, sprite, NB_GAME_CAR_W, NB_GAME_CAR_H, WHITE);
            if (car.off_track) {
                view_render.drawRect(car.x - 1, car.y - 1, NB_GAME_CAR_W + 2, NB_GAME_CAR_H + 2, WHITE);
            }
        }

        draw_hud();
    }
    else if (game_state == GAME_FINISHING) {
        draw_sky();
        draw_track();
        draw_finish_gate();

        const unsigned char *sprite = bitmap_car_straight;
        view_render.drawBitmap(car.x, car.y, sprite, NB_GAME_CAR_W, NB_GAME_CAR_H, WHITE);

        view_render.setTextSize(1);
        view_render.setTextColor(WHITE);
        view_render.setCursor(39, 0);
        view_render.print(car.speed);
        view_render.print("kmh");
    }
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
void scr_nobrake_game_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case SCREEN_ENTRY: {
        APP_DBG_SIG("SCREEN_ENTRY: NOBRAKE_GAME\n");

        task_post_pure_msg(NB_GAME_CAR_ID, NB_GAME_CAR_SETUP);
        task_post_pure_msg(NB_GAME_TRACK_ID, NB_GAME_TRACK_INIT);
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
            task_post_pure_msg(NB_GAME_TRACK_ID, NB_GAME_TRACK_UPDATE);
            break;
        }

        if (game_state == GAME_PLAY) {
            if (nb_game_car_is_off_track() ||
                nb_game_obstacle_has_collision() ||
                nb_game_track_is_timeout()) {
                game_state = GAME_OVER;
                BUZZER_PlayTones(tones_game_crash);
                timer_remove_attr(AC_TASK_DISPLAY_ID, NB_GAME_TIME_TICK);
                stop_steer_repeat();
                SCREEN_TRAN(scr_game_over_handle, &scr_game_over);
                break;
            }
            if (nb_game_track_is_finished()) {
                game_state = GAME_FINISHING;
                finish_frame = 0;
                throttle_hold = 0;
                nb_game_score_record_finish(nb_game_track_get_elapsed_s());
                BUZZER_PlayTones(tones_game_finish);
                stop_steer_repeat();
                task_post_pure_msg(NB_GAME_CAR_ID, NB_GAME_CAR_THROTTLE_RELEASE);
                break;
            }

            if (throttle_sound_wait > 0) {
                throttle_sound_wait--;
            }

            task_post_pure_msg(NB_GAME_CAR_ID, NB_GAME_CAR_UPDATE);
            task_post_pure_msg(NB_GAME_TRACK_ID, NB_GAME_TRACK_UPDATE);
            task_post_pure_msg(NB_GAME_OBSTACLE_ID, NB_GAME_OBSTACLE_UPDATE);
        }
    } break;

    case AC_DISPLAY_BUTTON_UP_PRESSED: {
        if (game_state == GAME_PLAY) {
            start_steer_repeat(1);
        }
    } break;

    case AC_DISPLAY_BUTTON_DOWN_PRESSED: {
        if (game_state == GAME_PLAY) {
            start_steer_repeat(-1);
        }
    } break;

    case AC_DISPLAY_BUTTON_UP_LONG_PRESSED: {
        if (game_state == GAME_PLAY) {
            start_steer_repeat(1);
        }
    } break;

    case AC_DISPLAY_BUTTON_DOWN_LONG_PRESSED: {
        if (game_state == GAME_PLAY) {
            start_steer_repeat(-1);
        }
    } break;

    case AC_DISPLAY_BUTTON_UP_RELEASED:
    case AC_DISPLAY_BUTTON_DOWN_RELEASED: {
        if (game_state == GAME_PLAY) {
            stop_steer_repeat();
        }
    } break;

    case NB_GAME_MOVE_LEFT_REPEAT: {
        repeat_steer(-1);
    } break;

    case NB_GAME_MOVE_RIGHT_REPEAT: {
        repeat_steer(1);
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
        stop_steer_repeat();
        task_post_pure_msg(NB_GAME_CAR_ID, NB_GAME_CAR_RESET);
        task_post_pure_msg(NB_GAME_OBSTACLE_ID, NB_GAME_OBSTACLE_RESET);
    } break;

    default:
        break;
    }
}

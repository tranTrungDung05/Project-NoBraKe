#include "nb_game_track.h"
#include "nb_game_car.h"

nb_game_track_t track;
nb_game_line_t view_lines[NB_GAME_VISIBLE_LINES];

static nb_game_config_t game_config = {
    0
};

static int8_t curve_lut[NB_GAME_CURVE_LUT_SIZE] = {
    0, 7, 10, 8, 3, -7, -11, -8,
    0, 9, 13, 6, -5, -12, -9, 4
};
static uint8_t curve_lut_len = NB_GAME_CURVE_LUT_SIZE;
static uint8_t curve_lut_gain = 2;

extern nb_game_car_t car;

static uint16_t get_time_limit(void) {
    static const uint16_t time_s[] = { 115, 95, 80 };
    return time_s[game_config.difficulty];
}

static int32_t get_track_length(void) {
    static const uint8_t length_percent[] = { 75, 75, 100 };
    return ((int32_t)NB_GAME_TRACK_LENGTH * length_percent[game_config.difficulty]) / 100;
}

static void update_view(void) {
    int32_t road_x = 0;
    int16_t road_dx = 0;
    int16_t max_y = LCD_HEIGHT;
    int32_t start_segment = track.pos / NB_GAME_SEGMENT_LENGTH;
    int16_t cam_z_offset = (int16_t)(track.pos - (start_segment * NB_GAME_SEGMENT_LENGTH));
    int32_t cam_world_x;
    uint8_t curve_ring = (uint8_t)((start_segment / 7) % curve_lut_len);
    uint8_t curve_step = (uint8_t)(start_segment % 7);
    uint8_t best_index = 0;
    int16_t best_delta = 127;
    const int16_t player_y = AXIS_Y_CAR + (NB_GAME_CAR_H / 2);

    track.camX = (car.x + (NB_GAME_CAR_W / 2)) - (LCD_WIDTH / 2);
    cam_world_x = ((int32_t)track.camX * 175L) >> 3;

    for (uint8_t i = 0; i < NB_GAME_VISIBLE_LINES; ++i) {
        int32_t dz = ((int32_t)(i + 1) * NB_GAME_SEGMENT_LENGTH) - cam_z_offset;
        nb_game_line_t *line = &view_lines[i];

        if (dz < 1) {
            dz = 1;
        }

        line->curve = (int16_t)curve_lut[curve_ring] * curve_lut_gain;
        line->x = (int16_t)road_x;
        line->y = 0;
        line->z = (int16_t)dz;

        int32_t scale = ((int32_t)NB_GAME_CAMERA_DEPTH_NUM * 256L) / dz;
        line->X = (int16_t)((LCD_WIDTH / 2) + ((scale * (line->x - cam_world_x)) >> 8));
        line->Y = (int16_t)((LCD_HEIGHT / 2) - ((scale * (line->y - NB_GAME_CAMERA_HEIGHT)) >> 8));
        line->W = (int16_t)((scale * NB_GAME_ROAD_WORLD_WIDTH * (LCD_WIDTH / 2)) >> 16);
        if (line->W < 1) {
            line->W = 1;
        }
        line->visible = line->Y < max_y;
        if (line->visible) {
            max_y = line->Y;
        }

        int16_t delta = line->Y - player_y;
        if (delta < 0) {
            delta = -delta;
        }
        if (delta < best_delta) {
            best_delta = delta;
            best_index = i;
        }

        road_x += road_dx;
        road_dx += line->curve;

        curve_step++;
        if (curve_step >= 7) {
            curve_step = 0;
            curve_ring++;
            if (curve_ring >= curve_lut_len) {
                curve_ring = 0;
            }
        }
    }

    track.player_center_x = view_lines[best_index].X;
    track.player_half_width = view_lines[best_index].W;
}

int16_t nb_game_track_get_road_center_x(void) {
    return track.player_center_x;
}

int16_t nb_game_track_get_road_half_width(void) {
    return track.player_half_width;
}

uint8_t nb_game_track_get_progress_percent(void) {
    uint32_t percent = ((uint32_t)track.pos * 100U) / (uint32_t)track.length;
    return percent > 100 ? 100 : (uint8_t)percent;
}

uint16_t nb_game_track_get_elapsed_s(void) {
    return track.elapsed_ms / 1000;
}

uint8_t nb_game_track_get_time_left_s(void) {
    uint16_t elapsed_s = nb_game_track_get_elapsed_s();
    if (elapsed_s >= track.time_limit_s) {
        return 0;
    }
    return (uint8_t)(track.time_limit_s - elapsed_s);
}

uint8_t nb_game_track_is_finished(void) {
    return track.pos >= track.length;
}

uint8_t nb_game_track_is_timeout(void) {
    return nb_game_track_get_time_left_s() == 0 && !nb_game_track_is_finished();
}

uint8_t nb_game_settings_get_difficulty(void) {
    return game_config.difficulty;
}

uint8_t nb_game_settings_get_obstacle_max(void) {
    return game_config.difficulty == 2 ? 2 : 1;
}

void nb_game_track_set_curve_lut(const int8_t *lut, uint8_t len, uint8_t gain) {
    if (lut == (const int8_t*)0 || len == 0) {
        return;
    }
    if (len > NB_GAME_CURVE_LUT_SIZE) {
        len = NB_GAME_CURVE_LUT_SIZE;
    }

    for (uint8_t i = 0; i < len; ++i) {
        curve_lut[i] = lut[i];
    }
    curve_lut_len = len;
    curve_lut_gain = gain == 0 ? 1 : gain;
}

void nb_game_settings_change_difficulty(int8_t delta) {
    int16_t next = (int16_t)game_config.difficulty + delta;
    if (next < 0) {
        next = 0;
    }
    if (next > 2) {
        next = 2;
    }
    game_config.difficulty = (uint8_t)next;
}

void nb_game_track_handle(ak_msg_t *msg) {
    switch (msg->sig) {
    case NB_GAME_TRACK_INIT: {
        track.pos = 0;
        track.length = get_track_length();
        track.camX = 0;
        track.player_center_x = LCD_WIDTH / 2;
        track.player_half_width = LCD_WIDTH / 2;
        track.scroll_speed = 0;
        track.elapsed_ms = 0;
        track.time_limit_s = get_time_limit();
        update_view();
    } break;

    case NB_GAME_TRACK_UPDATE: {
        track.scroll_speed = (uint8_t)(((uint16_t)car.speed * 13U) / 20U);
        track.pos += track.scroll_speed;
        track.elapsed_ms += NB_GAME_TIME_TICK_INTERVAL;
        update_view();
    } break;

    default:
        break;
    }
}

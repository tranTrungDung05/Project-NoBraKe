#ifndef __NB_GAME_TRACK_H__
#define __NB_GAME_TRACK_H__

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"
#include "app.h"
#include "task_list.h"

#define NB_GAME_VISIBLE_LINES       (34)
#define NB_GAME_SEGMENT_LENGTH      (200)
#define NB_GAME_TRACK_LENGTH        (60000)
#define NB_GAME_ROAD_WORLD_WIDTH    (1400)
#define NB_GAME_CAMERA_HEIGHT       (120)
#define NB_GAME_CAMERA_DEPTH_NUM    (84)
#define NB_GAME_LANE_COUNT          (3)
#define NB_GAME_CURVE_LUT_SIZE      (16)

typedef struct {
    uint8_t difficulty;
} nb_game_config_t;

typedef struct {
    int16_t x, y, z;
    int16_t X, Y, W;
    int16_t curve;
    uint8_t visible;
} nb_game_line_t;

typedef struct {
    int16_t camX;
    int16_t player_center_x;
    int16_t player_half_width;
    int32_t pos;
    int32_t length;
    uint8_t scroll_speed;
    uint16_t elapsed_ms;
    uint16_t time_limit_s;
} nb_game_track_t;

extern nb_game_track_t track;
extern nb_game_line_t view_lines[NB_GAME_VISIBLE_LINES];

void nb_game_track_handle(ak_msg_t *msg);
int16_t nb_game_track_get_road_center_x(void);
int16_t nb_game_track_get_road_half_width(void);
uint8_t nb_game_track_get_progress_percent(void);
uint16_t nb_game_track_get_elapsed_s(void);
uint8_t nb_game_track_get_time_left_s(void);
uint8_t nb_game_track_is_finished(void);
uint8_t nb_game_track_is_timeout(void);
uint8_t nb_game_settings_get_obstacle_max(void);
uint8_t nb_game_settings_get_difficulty(void);
void nb_game_settings_change_difficulty(int8_t delta);
void nb_game_track_set_curve_lut(const int8_t *lut, uint8_t len, uint8_t gain);

#endif

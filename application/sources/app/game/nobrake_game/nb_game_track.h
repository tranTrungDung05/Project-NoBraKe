#ifndef __NB_GAME_TRACK_H__
#define __NB_GAME_TRACK_H__

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"
#include "app.h"
#include "task_list.h"
#include <stdlib.h>

#include "nb_game_car.h"
#include "nb_game_obstacle.h"

#define NB_GAME_VISIBLE_LINES       (24)
#define NB_GAME_SEGMENT_LENGTH      (200)
#define NB_GAME_TRACK_LENGTH        (60000)
#define NB_GAME_ROAD_WORLD_WIDTH    (1560)
#define NB_GAME_CAMERA_HEIGHT       (120)
#define NB_GAME_CAMERA_DEPTH_NUM    (56)
#define NB_GAME_LANE_COUNT          (3)
#define NB_GAME_CURVE_LUT_SIZE      (16)
#define NB_GAME_TREE_COUNT          (6)

typedef struct {
    int16_t x, y, z;
    int16_t X, Y, W;
    int16_t curve;
    bool visible;
} nb_game_line_t;

typedef struct {
    int32_t z;
    int16_t x;
    int16_t y;
    int8_t side;
    uint8_t active;
    uint8_t view_index;
    uint8_t size;
} nb_game_tree_t;

typedef struct {
    int32_t pos;
    int32_t length;
    uint32_t elapsed_ms;
    int16_t camX;
    int16_t player_center_x;
    int16_t player_half_width;
    uint16_t time_limit_s;
} nb_game_track_t;

extern nb_game_track_t track;
extern nb_game_line_t view_lines[NB_GAME_VISIBLE_LINES];
extern nb_game_tree_t view_trees[NB_GAME_TREE_COUNT];

int16_t nb_game_track_get_lane_center_x(const nb_game_line_t *line, uint8_t lane_index);
uint8_t nb_game_track_get_random(uint8_t max);
uint16_t nb_game_track_get_elapsed_s(void);
uint8_t nb_game_track_get_time_left_s(void);
uint8_t nb_game_settings_get_difficulty(void);
uint8_t nb_game_settings_get_obstacle_max(void);
void nb_game_settings_change_difficulty(int8_t delta);

void nb_game_track_handle(ak_msg_t *msg);

#endif //__NB_GAME_TRACK_H__

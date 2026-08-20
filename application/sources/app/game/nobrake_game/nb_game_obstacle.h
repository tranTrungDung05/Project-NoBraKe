#ifndef __NB_GAME_OBSTACLE_H__
#define __NB_GAME_OBSTACLE_H__

#include "fsm.h"
#include "message.h"
#include "app.h"

#include "nb_game_track.h"
#include <string.h>

#define NB_GAME_OBSTACLE_COUNT      (4)
#define NB_GAME_OBSTACLE_W          (8)
#define NB_GAME_OBSTACLE_H          (8)

enum {
    NB_GAME_OBSTACLE_STONE,
    NB_GAME_OBSTACLE_BARRIER,
    NB_GAME_OBSTACLE_CAR,
    NB_GAME_OBSTACLE_TYPE_COUNT
};

typedef struct {
    int32_t z;
    uint8_t lane;
    uint8_t type;
    uint8_t active;
    uint8_t view_index;
    int16_t x;
    int16_t y;
} nb_game_obstacle_t;

void nb_game_obstacle_handle(ak_msg_t *msg);
uint8_t nb_game_obstacle_count(void);
const nb_game_obstacle_t *nb_game_obstacle_at(uint8_t index);
void nb_game_obstacle_update_world(void);

extern nb_game_obstacle_t nb_game_obstacles[NB_GAME_OBSTACLE_COUNT];
extern uint8_t NB_GAME_OBSTACLE_VISIBLE;

#endif // __NB_GAME_OBSTACLE_H__

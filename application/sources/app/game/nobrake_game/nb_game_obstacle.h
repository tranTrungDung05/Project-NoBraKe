#ifndef __NB_GAME_OBSTACLE_H__
#define __NB_GAME_OBSTACLE_H__

#include "fsm.h"
#include "message.h"

#include "app.h"
#include "app_dbg.h"

#define NB_GAME_OBSTACLE_COUNT      (4)

typedef struct {
    int32_t z;
    uint8_t lane_mask;
    uint8_t active;
} nb_game_obstacle_t;

void nb_game_obstacle_handle(ak_msg_t *msg);
uint8_t nb_game_obstacle_has_collision(void);
uint8_t nb_game_obstacle_get_view_index(const nb_game_obstacle_t *obstacle);

extern nb_game_obstacle_t nb_game_obstacles[NB_GAME_OBSTACLE_COUNT];
extern uint8_t NB_GAME_OBSTACLE_VISIBLE;

#endif // __NB_GAME_OBSTACLE_H__

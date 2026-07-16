#ifndef __NB_GAME_CAR_H__
#define __NB_GAME_CAR_H__

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"
#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"

#define NB_GAME_CAR_W       (18)
#define NB_GAME_CAR_H       (18)
#define BASE_STEER_STEP     (4)
#define MIN_STEER_STEP      (1)
#define NB_GAME_SPEED_MAX   (180)

#define AXIS_X_CAR          (55)
#define AXIS_Y_CAR          (43)

typedef struct {
    bool visible;
    int16_t x;
    int16_t y;
    uint8_t speed;
    uint8_t throttle;
    uint8_t off_track;
    int8_t hard_steer;
    int8_t steer_step;
    uint8_t action_image;
} nb_game_car_t;

extern nb_game_car_t car;

enum {
    NB_GAME_CAR_ACTION_IMAGE_LEFT = 0,
    NB_GAME_CAR_ACTION_IMAGE_STRAIGHT = 1,
    NB_GAME_CAR_ACTION_IMAGE_RIGHT = 2
};

void nb_game_car_handle(ak_msg_t *msg);
uint8_t nb_game_car_is_off_track(void);

#endif

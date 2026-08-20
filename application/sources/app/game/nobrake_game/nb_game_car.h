#ifndef __NB_GAME_CAR_H__
#define __NB_GAME_CAR_H__

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"
#include "app.h"
#include "task_list.h"
#include "task_display.h"

#include "nb_game_track.h"
#include "nb_game_obstacle.h"

#define NB_GAME_CAR_W	  (24)
#define NB_GAME_CAR_H	  (24)
#define BASE_STEER_STEP	  (4)
#define MIN_STEER_STEP	  (1)
#define NB_GAME_SPEED_MAX (180)

#define AXIS_X_CAR (55)
#define AXIS_Y_CAR (40)

typedef struct {
	bool visible;
	int16_t x, y;
	uint8_t speed;
	uint8_t throttle;
	int8_t hard_steer;
	int8_t steer_step;
	bool is_crash; /* 0: no crash --  1: crash     -> serve for game state   */
	bool state;	   /* 0: stopping --  1: running   -> serve for track update */
} nb_game_car_t;

extern nb_game_car_t car;

void nb_game_car_handle(ak_msg_t *msg);

#endif	  //__NB_GAME_CAR_H__

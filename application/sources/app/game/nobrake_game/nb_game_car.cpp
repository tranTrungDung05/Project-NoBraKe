#include "nb_game_car.h"

nb_game_car_t car;

static void post_world_update(void) {
	task_post_pure_msg(NB_GAME_TRACK_ID, NB_GAME_TRACK_UPDATE);
}

static void update_steer_step(void) {
	car.hard_steer = car.speed / 12;
	car.steer_step = BASE_STEER_STEP - (car.hard_steer / 4);
	if (car.steer_step < MIN_STEER_STEP) {
		car.steer_step = MIN_STEER_STEP;
	}
}

static void update_speed(void) {
	if (car.throttle) {
		if (car.speed < NB_GAME_SPEED_MAX) {
			car.speed += 3;
		}
	}
	else if (car.speed > 0) {
		car.speed -= car.speed > 2 ? 2 : car.speed;
	}

	car.state = (car.speed > 0 || car.throttle) ? 1 : 0;
	update_steer_step();
}

static void finish_brake(void) {
	car.throttle = 0;
	if (car.speed > 12) {
		car.speed -= 12;
	}
	else {
		car.speed = 0;
	}
	update_steer_step();
}

static void update_track_state(void) {
	int16_t left	   = track.player_center_x - track.player_half_width;
	int16_t right	   = track.player_center_x + track.player_half_width;
	int16_t car_center = car.x + (NB_GAME_CAR_W / 2);

	car.is_crash = (car_center < left || car_center > right) ? 1 : 0;
}

static uint8_t has_obstacle_collision(void) {
	int16_t car_left   = car.x + 5;
	int16_t car_top	   = car.y + 12;
	int16_t car_right  = car.x + NB_GAME_CAR_W - 5;
	int16_t car_bottom = car.y + NB_GAME_CAR_H - 2;

	for (uint8_t i = 0; i < nb_game_obstacle_count(); ++i) {
		const nb_game_obstacle_t *obstacle = nb_game_obstacle_at(i);
		if (obstacle == 0 || obstacle->view_index == 0xFF) {
			continue;
		}

		int16_t obstacle_left	= obstacle->x - (NB_GAME_OBSTACLE_W / 2) + 2;
		int16_t obstacle_top	= obstacle->y - NB_GAME_OBSTACLE_H + 4;
		int16_t obstacle_right	= obstacle->x + (NB_GAME_OBSTACLE_W / 2) - 2;
		int16_t obstacle_bottom = obstacle->y - 2;

		if (car_right < obstacle_left || car_left > obstacle_right) {
			continue;
		}
		if (car_bottom < obstacle_top || car_top > obstacle_bottom) {
			continue;
		}
		return 1;
	}

	return 0;
}

void nb_game_car_handle(ak_msg_t *msg) {
	switch (msg->sig) {
	case NB_GAME_CAR_SETUP: {
		car.x		   = AXIS_X_CAR;
		car.y		   = AXIS_Y_CAR;
		car.visible	   = true;
		car.speed	   = 0;
		car.throttle   = 0;
		car.is_crash   = 0;
		car.hard_steer = 0;
		car.steer_step = BASE_STEER_STEP;
		car.state	   = 0;
	} break;

	case NB_GAME_CAR_THROTTLE: {
		car.throttle = 1;
		car.state	 = 1;
	} break;

	case NB_GAME_CAR_THROTTLE_RELEASE: {
		car.throttle = 0;
		if (car.speed == 0) {
			car.state = 0;
		}
	} break;

	case NB_GAME_CAR_UPDATE: {
		update_speed();

		if (car.speed == 0) {
			car.is_crash = 0;
			post_world_update();
			break;
		}

		post_world_update();
	} break;

	case NB_GAME_CAR_CHECK_CRASH: {
		update_track_state();
		if (car.is_crash == false) {
			car.is_crash = has_obstacle_collision();
		}
		if (car.is_crash == true) {
			car.state = 0;
		}
	} break;

	case NB_GAME_CAR_TURN_LEFT: {
		if (car.speed > 0) {
			car.x -= car.steer_step;
			if (car.x < -NB_GAME_CAR_W)
				car.x = -NB_GAME_CAR_W;
		}
	} break;

	case NB_GAME_CAR_TURN_RIGHT: {
		if (car.speed > 0) {
			car.x += car.steer_step;
			if (car.x > LCD_WIDTH)
				car.x = LCD_WIDTH;
		}
	} break;

	case NB_GAME_CAR_FINISH_BRAKE: {
		finish_brake();
		car.state = 0;
	} break;

	case NB_GAME_CAR_RESET: {
		car.visible	 = false;
		car.throttle = 0;
		car.speed	 = 0;
		car.state	 = 0;
		car.is_crash = 0;
	} break;

	default:
		break;
	}
}

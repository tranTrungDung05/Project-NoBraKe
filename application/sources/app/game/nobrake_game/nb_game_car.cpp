#include "nb_game_car.h"
#include "nb_game_track.h"

nb_game_car_t car;

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
    int16_t left = nb_game_track_get_road_center_x() - nb_game_track_get_road_half_width();
    int16_t right = nb_game_track_get_road_center_x() + nb_game_track_get_road_half_width();
    int16_t car_center = car.x + (NB_GAME_CAR_W / 2);

    car.off_track = (car_center < left || car_center > right) ? 1 : 0;
    if (car.off_track && car.speed > 10) {
        car.speed -= 10;
    }
}

void nb_game_car_handle(ak_msg_t *msg) {
    switch (msg->sig) {
    case NB_GAME_CAR_SETUP: {
        car.x = AXIS_X_CAR;
        car.y = AXIS_Y_CAR;
        car.visible = true;
        car.speed = 0;
        car.throttle = 0;
        car.off_track = 0;
        car.hard_steer = 0;
        car.steer_step = BASE_STEER_STEP;
        car.action_image = NB_GAME_CAR_ACTION_IMAGE_STRAIGHT;
    } break;

    case NB_GAME_CAR_THROTTLE: {
        car.throttle = 1;
    } break;

    case NB_GAME_CAR_THROTTLE_RELEASE: {
        car.throttle = 0;
    } break;

    case NB_GAME_CAR_UPDATE: {
        update_speed();
        update_track_state();
    } break;

    case NB_GAME_CAR_ACTION_STRAIGHT: {
        car.action_image = NB_GAME_CAR_ACTION_IMAGE_STRAIGHT;
    } break;

    case NB_GAME_CAR_ACTION_LEFT: {
        car.x -= car.steer_step;
        car.action_image = NB_GAME_CAR_ACTION_IMAGE_LEFT;
        if (car.x < -NB_GAME_CAR_W) car.x = -NB_GAME_CAR_W;
    } break;

    case NB_GAME_CAR_ACTION_RIGHT: {
        car.x += car.steer_step;
        car.action_image = NB_GAME_CAR_ACTION_IMAGE_RIGHT;
        if (car.x > LCD_WIDTH) car.x = LCD_WIDTH;
    } break;

    case NB_GAME_CAR_FINISH_BRAKE: {
        finish_brake();
    } break;

    case NB_GAME_CAR_RESET: {
        car.visible = false;
        car.throttle = 0;
        car.speed = 0;
    } break;

    default:
        break;
    }
}

uint8_t nb_game_car_is_off_track(void) {
    return car.off_track;
}

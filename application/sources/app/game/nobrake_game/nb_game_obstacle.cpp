#include "nb_game_obstacle.h"

#include "nb_game_car.h"
#include "nb_game_track.h"
#include <string.h>

nb_game_obstacle_t nb_game_obstacles[NB_GAME_OBSTACLE_COUNT];
uint8_t NB_GAME_OBSTACLE_VISIBLE;

static uint8_t obstacle_head;
static uint8_t obstacle_tail;
static int32_t next_obstacle_z;
static uint32_t obstacle_rng = 1;

static uint32_t next_rand(void) {
    obstacle_rng ^= obstacle_rng << 13;
    obstacle_rng ^= obstacle_rng >> 17;
    obstacle_rng ^= obstacle_rng << 5;
    return obstacle_rng;
}

static uint8_t make_lane_mask(void) {
    uint8_t first_lane = (uint8_t)(next_rand() % NB_GAME_LANE_COUNT);
    uint8_t mask = (uint8_t)(1U << first_lane);

    if (nb_game_settings_get_difficulty() == 0) {
        return mask;
    }

    if ((next_rand() & 0x01U) != 0U) {
        uint8_t second_lane = (uint8_t)(next_rand() % NB_GAME_LANE_COUNT);
        if (second_lane == first_lane) {
            second_lane = (uint8_t)((second_lane + 1U) % NB_GAME_LANE_COUNT);
        }
        mask |= (uint8_t)(1U << second_lane);
    }

    return mask;
}

static uint16_t get_spawn_gap(void) {
    static const uint8_t spacing_segments[] = { 22, 12, 8 };
    return (uint16_t)(NB_GAME_SEGMENT_LENGTH * spacing_segments[nb_game_settings_get_difficulty()]);
}

static void push_obstacle(int32_t z) {
    nb_game_obstacle_t *obstacle = &nb_game_obstacles[obstacle_tail];
    obstacle->z = z;
    obstacle->lane_mask = make_lane_mask();
    obstacle->active = 1;

    obstacle_tail = (uint8_t)((obstacle_tail + 1U) % NB_GAME_OBSTACLE_COUNT);
    if (NB_GAME_OBSTACLE_VISIBLE < NB_GAME_OBSTACLE_COUNT) {
        NB_GAME_OBSTACLE_VISIBLE++;
    }
    else {
        obstacle_head = (uint8_t)((obstacle_head + 1U) % NB_GAME_OBSTACLE_COUNT);
    }
}

static void pop_obstacle(void) {
    nb_game_obstacles[obstacle_head].active = 0;
    obstacle_head = (uint8_t)((obstacle_head + 1U) % NB_GAME_OBSTACLE_COUNT);
    if (NB_GAME_OBSTACLE_VISIBLE > 0) {
        NB_GAME_OBSTACLE_VISIBLE--;
    }
}

static void fill_obstacles(void) {
    uint8_t target = nb_game_settings_get_obstacle_max();
    if (target > NB_GAME_OBSTACLE_COUNT) {
        target = NB_GAME_OBSTACLE_COUNT;
    }

    while (NB_GAME_OBSTACLE_VISIBLE < target) {
        push_obstacle(next_obstacle_z);
        next_obstacle_z += get_spawn_gap();
    }
}

uint8_t nb_game_obstacle_get_view_index(const nb_game_obstacle_t *obstacle) {
    if (!obstacle->active || obstacle->z < track.pos) {
        return 0xFF;
    }

    int32_t rel_z = obstacle->z - track.pos;
    uint8_t view_index = (uint8_t)(rel_z / NB_GAME_SEGMENT_LENGTH);
    if (view_index >= NB_GAME_VISIBLE_LINES) {
        return 0xFF;
    }
    return view_index;
}

uint8_t nb_game_obstacle_has_collision(void) {
    int16_t car_center = car.x + (NB_GAME_CAR_W / 2);
    int16_t car_y = car.y + (NB_GAME_CAR_H / 2);

    for (uint8_t i = 0; i < NB_GAME_OBSTACLE_COUNT; ++i) {
        const nb_game_obstacle_t *obstacle = &nb_game_obstacles[i];
        uint8_t view_index = nb_game_obstacle_get_view_index(obstacle);
        if (view_index == 0xFF) {
            continue;
        }

        int16_t obstacle_y = view_lines[view_index].Y;
        if (obstacle_y < car_y - 7 || obstacle_y > car_y + 7) {
            continue;
        }

        int16_t lane_width = (view_lines[view_index].W * 2) / NB_GAME_LANE_COUNT;
        int16_t left = view_lines[view_index].X - view_lines[view_index].W;
        for (uint8_t lane = 0; lane < NB_GAME_LANE_COUNT; ++lane) {
            if ((obstacle->lane_mask & (1U << lane)) == 0U) {
                continue;
            }
            int16_t obstacle_x = left + (lane_width / 2) + (lane_width * lane);
            if (car_center >= obstacle_x - 7 && car_center <= obstacle_x + 7) {
                return 1;
            }
        }
    }
    return 0;
}

void nb_game_obstacle_handle(ak_msg_t *msg) {
    switch (msg->sig) {
    case NB_GAME_OBSTACLE_SETUP: {
        NB_GAME_OBSTACLE_VISIBLE = 0;
        obstacle_head = 0;
        obstacle_tail = 0;
        next_obstacle_z = NB_GAME_SEGMENT_LENGTH * 10;
        memset(nb_game_obstacles, 0, sizeof(nb_game_obstacles));
        fill_obstacles();
    } break;

    case NB_GAME_OBSTACLE_UPDATE: {
        while (NB_GAME_OBSTACLE_VISIBLE > 0 &&
               nb_game_obstacles[obstacle_head].z + NB_GAME_SEGMENT_LENGTH < track.pos) {
            pop_obstacle();
        }
        fill_obstacles();
    } break;

    case NB_GAME_OBSTACLE_RESET: {
        NB_GAME_OBSTACLE_VISIBLE = 0;
        obstacle_head = 0;
        obstacle_tail = 0;
        memset(nb_game_obstacles, 0, sizeof(nb_game_obstacles));
    } break;

    default:
        break;
    }
}

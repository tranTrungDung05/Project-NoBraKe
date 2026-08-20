#include "nb_game_obstacle.h"

nb_game_obstacle_t nb_game_obstacles[NB_GAME_OBSTACLE_COUNT];
uint8_t NB_GAME_OBSTACLE_VISIBLE;

static uint8_t obstacle_head;
static uint8_t obstacle_tail;
static int32_t next_obstacle_z;

static uint16_t get_spawn_gap(void)
{
	static const uint8_t spacing_segments[] = {22, 12, 8};
	return (uint16_t)(NB_GAME_SEGMENT_LENGTH * spacing_segments[nb_game_settings_get_difficulty()]);
}

static void push_obstacle(int32_t z)
{
	nb_game_obstacle_t* obstacle = &nb_game_obstacles[obstacle_tail];
	obstacle->z = z;
	obstacle->lane = nb_game_track_get_random(NB_GAME_LANE_COUNT);
	obstacle->type = nb_game_track_get_random(NB_GAME_OBSTACLE_TYPE_COUNT);
	obstacle->active = 1;

	obstacle_tail = (uint8_t)((obstacle_tail + 1) % NB_GAME_OBSTACLE_COUNT);
	if (NB_GAME_OBSTACLE_VISIBLE < NB_GAME_OBSTACLE_COUNT)
	{
		NB_GAME_OBSTACLE_VISIBLE++;
	}
	else
	{
		obstacle_head = (uint8_t)((obstacle_head + 1) % NB_GAME_OBSTACLE_COUNT);
	}
}

static void pop_obstacle(void)
{
	nb_game_obstacles[obstacle_head].active = 0;
	obstacle_head = (uint8_t)((obstacle_head + 1) % NB_GAME_OBSTACLE_COUNT);
	if (NB_GAME_OBSTACLE_VISIBLE > 0)
	{
		NB_GAME_OBSTACLE_VISIBLE--;
	}
}

static void fill_obstacles(void)
{
	uint8_t target = nb_game_settings_get_obstacle_max();
	if (target > NB_GAME_OBSTACLE_COUNT)
	{
		target = NB_GAME_OBSTACLE_COUNT;
	}

	while (NB_GAME_OBSTACLE_VISIBLE < target)
	{
		push_obstacle(next_obstacle_z);
		next_obstacle_z += get_spawn_gap();
	}
}

uint8_t nb_game_obstacle_count(void)
{
	return NB_GAME_OBSTACLE_VISIBLE;
}

const nb_game_obstacle_t* nb_game_obstacle_at(uint8_t index)
{
	if (index >= NB_GAME_OBSTACLE_VISIBLE)
	{
		return 0;
	}
	return &nb_game_obstacles[(obstacle_head + index) % NB_GAME_OBSTACLE_COUNT];
}

void nb_game_obstacle_update_world(void)
{
	while (NB_GAME_OBSTACLE_VISIBLE > 0 &&
	       nb_game_obstacles[obstacle_head].z + NB_GAME_SEGMENT_LENGTH < track.pos)
	{
		pop_obstacle();
	}
	fill_obstacles();

	for (uint8_t i = 0; i < NB_GAME_OBSTACLE_COUNT; ++i)
	{
		nb_game_obstacle_t* obstacle = &nb_game_obstacles[i];
		obstacle->view_index = 255;
		obstacle->x = 0;
		obstacle->y = 0;

		if (!obstacle->active || obstacle->z < track.pos)
		{
			continue;
		}

		int32_t rel_z = obstacle->z - track.pos;
		uint8_t view_index = (uint8_t)(rel_z / NB_GAME_SEGMENT_LENGTH);
		if (view_index >= NB_GAME_VISIBLE_LINES)
		{
			continue;
		}

		const nb_game_line_t* line = &view_lines[view_index];
		if (obstacle->lane >= NB_GAME_LANE_COUNT)
		{
			continue;
		}

		int16_t lane_x = nb_game_track_get_lane_center_x(line, obstacle->lane);
		obstacle->view_index = view_index;
		obstacle->x = lane_x;
		obstacle->y = line->Y;
	}
}

void nb_game_obstacle_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case NB_GAME_OBSTACLE_SETUP:
	{
		NB_GAME_OBSTACLE_VISIBLE = 0;
		obstacle_head = 0;
		obstacle_tail = 0;
		next_obstacle_z = NB_GAME_SEGMENT_LENGTH * 10;
		memset(nb_game_obstacles, 0, sizeof(nb_game_obstacles));
		fill_obstacles();
		nb_game_obstacle_update_world();
	}
	break;

	case NB_GAME_OBSTACLE_UPDATE:
	{
		nb_game_obstacle_update_world();
		task_post_pure_msg(NB_GAME_CAR_ID, NB_GAME_CAR_CHECK_CRASH);
	}
	break;

	case NB_GAME_OBSTACLE_RESET:
	{
		NB_GAME_OBSTACLE_VISIBLE = 0;
		obstacle_head = 0;
		obstacle_tail = 0;
		memset(nb_game_obstacles, 0, sizeof(nb_game_obstacles));
	}
	break;

	default:
		break;
	}
}

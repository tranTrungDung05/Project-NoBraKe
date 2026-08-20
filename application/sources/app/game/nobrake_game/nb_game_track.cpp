#include "nb_game_track.h"

nb_game_track_t track;
nb_game_line_t view_lines[NB_GAME_VISIBLE_LINES];
nb_game_tree_t view_trees[NB_GAME_TREE_COUNT];

static uint8_t tree_head;
static uint8_t tree_tail;
static uint8_t tree_visible;
static uint8_t tree_pattern;
static int32_t next_tree_z;

static const uint16_t tree_gaps[] = {520, 760, 610, 900, 570, 700};
static const int8_t tree_sides[] = {-1, 1, 1, -1, -1, 1};

static uint8_t difficulty;

static int8_t curve_lut[NB_GAME_CURVE_LUT_SIZE] = {
    0, 7, 10, 8, 3, -7, -11, -8,
    0, 9, 13, 6, -5, -12, -9, 4};
static const uint8_t curve_lut_gain = 2;

extern nb_game_car_t car;

/* Track setup values */
static uint16_t get_time_limit(void)
{
	static const uint16_t time_s[] = {90, 70, 60};
	return time_s[difficulty];
}

static int32_t get_track_length(void)
{
	static const uint8_t length_percent[] = {75, 75, 100};
	return ((int32_t)NB_GAME_TRACK_LENGTH * length_percent[difficulty]) / 100;
}

static void push_tree(void)
{
	nb_game_tree_t* tree = &view_trees[tree_tail];
	tree->z = next_tree_z;
	tree->side = tree_sides[tree_pattern];
	tree->active = 1;
	tree->view_index = 0xFF;
	tree_tail = (uint8_t)((tree_tail + 1U) % NB_GAME_TREE_COUNT);
	if (tree_visible < NB_GAME_TREE_COUNT)
	{
		tree_visible++;
	}
	else
	{
		tree_head = (uint8_t)((tree_head + 1U) % NB_GAME_TREE_COUNT);
	}
	next_tree_z += tree_gaps[tree_pattern];
	tree_pattern = (uint8_t)((tree_pattern + 1U) % (sizeof(tree_gaps) / sizeof(tree_gaps[0])));
}

static void pop_tree(void)
{
	view_trees[tree_head].active = 0;
	tree_head = (uint8_t)((tree_head + 1U) % NB_GAME_TREE_COUNT);
	if (tree_visible > 0)
	{
		tree_visible--;
	}
}

static void fill_trees(void)
{
	while (tree_visible < NB_GAME_TREE_COUNT)
	{
		push_tree();
	}
}

/* Projection: convert world track data to screen-space lines */
static void update_view(void)
{
	/* Camera and projection state */
	int32_t road_x = 0;
	int16_t road_dx = 0;
	int16_t max_y = LCD_HEIGHT;
	int32_t start_segment = track.pos / NB_GAME_SEGMENT_LENGTH;
	int16_t cam_z_offset = (int16_t)(track.pos - (start_segment * NB_GAME_SEGMENT_LENGTH));
	int32_t cam_world_x;
	uint8_t curve_ring = (uint8_t)((start_segment / 7) % NB_GAME_CURVE_LUT_SIZE);
	uint8_t curve_step = (uint8_t)(start_segment % 7);
	uint8_t best_index = 0;
	int16_t best_delta = 127;
	const int16_t player_y = AXIS_Y_CAR + (NB_GAME_CAR_H / 2);

	/* Put the camera in the center of the car */
	track.camX = (car.x + (NB_GAME_CAR_W / 2)) - (LCD_WIDTH / 2);
	cam_world_x = ((int32_t)track.camX * NB_GAME_ROAD_WORLD_WIDTH) / LCD_WIDTH;

	/* Project each visible road segment into screen space */
	for (uint8_t i = 0; i < NB_GAME_VISIBLE_LINES; ++i)
	{
		int32_t dz = ((int32_t)(i + 1) * NB_GAME_SEGMENT_LENGTH) - cam_z_offset;
		nb_game_line_t* line = &view_lines[i];

		/* Keep depth valid so projection math never divides by zero */
		if (dz < 1)
		{
			dz = 1;
		}

		/* Build the road segment in world space */
		line->curve = (int16_t)curve_lut[curve_ring] * curve_lut_gain;
		line->x = (int16_t)road_x;
		line->y = 0;
		line->z = (int16_t)dz;

		/* Convert world coordinates to screen coordinates */
		int32_t scale = ((int32_t)NB_GAME_CAMERA_DEPTH_NUM * 256L) / dz;
		line->X = (int16_t)((LCD_WIDTH / 2) + ((scale * (line->x - cam_world_x)) >> 8));
		line->Y = (int16_t)((LCD_HEIGHT / 2) - ((scale * (line->y - NB_GAME_CAMERA_HEIGHT)) >> 8));
		line->W = (int16_t)((scale * NB_GAME_ROAD_WORLD_WIDTH * (LCD_WIDTH / 2)) >> 16);
		if (line->W < 1)
		{
			line->W = 1;
		}

		/* Only keep the closest line visible at each screen row */
		line->visible = line->Y < max_y;
		if (line->visible)
		{
			max_y = line->Y;
		}

		/* Pick the projected line closest to the car for collision helpers */
		int16_t delta = line->Y - player_y;
		if (delta < 0)
		{
			delta = -delta;
		}
		if (delta < best_delta)
		{
			best_delta = delta;
			best_index = i;
		}

		road_x += road_dx;
		road_dx += line->curve;

		curve_step++;
		if (curve_step >= 7)
		{
			curve_step = 0;
			curve_ring++;
			if (curve_ring >= NB_GAME_CURVE_LUT_SIZE)
			{
				curve_ring = 0;
			}
		}
	}

	/* Cache the road line under the car for gameplay checks */
	track.player_center_x = view_lines[best_index].X;
	track.player_half_width = view_lines[best_index].W + 10;
}

static void update_tree_view(void)
{
	for (uint8_t i = 0; i < NB_GAME_TREE_COUNT; ++i)
	{
		view_trees[i].view_index = 0xFF;
		view_trees[i].size = 0;
	}

	while (tree_visible > 0 && view_trees[tree_head].z < track.pos)
	{
		pop_tree();
	}
	fill_trees();

	for (uint8_t i = 0; i < NB_GAME_TREE_COUNT; ++i)
	{
		nb_game_tree_t* tree = &view_trees[i];
		if (!tree->active || tree->z < track.pos)
		{
			continue;
		}

		uint8_t view_index = (uint8_t)((tree->z - track.pos) / NB_GAME_SEGMENT_LENGTH);
		if (view_index >= NB_GAME_VISIBLE_LINES)
		{
			continue;
		}

		const nb_game_line_t* line = &view_lines[view_index];
		if (!line->visible || line->Y <= 16)
		{
			continue;
		}

		tree->view_index = view_index;
		tree->x = line->X + (tree->side * (line->W + (line->W / 2) + 20));
		tree->y = line->Y;
		tree->size = (uint8_t)(4 + (((NB_GAME_VISIBLE_LINES - view_index) * 20) / NB_GAME_VISIBLE_LINES));
	}
}

/* Geometry helper: lane center on a projected line */
int16_t nb_game_track_get_lane_center_x(const nb_game_line_t* line, uint8_t lane_index)
{
	if (line == (const nb_game_line_t*)0)
	{
		return 0;
	}
	if (lane_index >= NB_GAME_LANE_COUNT)
	{
		lane_index = NB_GAME_LANE_COUNT - 1U;
	}

	int16_t left = line->X - line->W;
	return (int16_t)(left + (((int32_t)line->W * ((lane_index * 2U) + 1U)) / NB_GAME_LANE_COUNT));
}

uint8_t nb_game_track_get_random(uint8_t max)
{
	if (max == 0)
	{
		return 0;
	}
	return rand() % max;
}

/* Gameplay queries */
uint16_t nb_game_track_get_elapsed_s(void)
{
	return track.elapsed_ms / 1000;
}

uint8_t nb_game_track_get_time_left_s(void)
{
	uint16_t elapsed_s = nb_game_track_get_elapsed_s();
	if (elapsed_s >= track.time_limit_s)
	{
		return 0;
	}
	return (uint8_t)(track.time_limit_s - elapsed_s);
}

uint8_t nb_game_settings_get_difficulty(void)
{
	return difficulty;
}

uint8_t nb_game_settings_get_obstacle_max(void)
{
	return difficulty == 2 ? 2 : 1;
}

/* Gameplay settings */
void nb_game_settings_change_difficulty(int8_t delta)
{
	int16_t next = (int16_t)difficulty + delta;
	if (next < 0)
	{
		next = 0;
	}
	if (next > 2)
	{
		next = 2;
	}
	difficulty = (uint8_t)next;
}

void nb_game_track_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case NB_GAME_TRACK_SETUP:
	{
		track.pos = 0;
		track.length = get_track_length();
		track.camX = 0;
		track.player_center_x = LCD_WIDTH / 2;
		track.player_half_width = LCD_WIDTH / 2;
		track.elapsed_ms = 0;
		track.time_limit_s = get_time_limit();
		tree_head = 0;
		tree_tail = 0;
		tree_visible = 0;
		tree_pattern = 0;
		next_tree_z = NB_GAME_SEGMENT_LENGTH * 3;
		for (uint8_t i = 0; i < NB_GAME_TREE_COUNT; ++i)
		{
			view_trees[i].active = 0;
		}
		fill_trees();
		update_view();
		update_tree_view();
	}
	break;

	case NB_GAME_TRACK_UPDATE:
	{
		track.elapsed_ms += NB_GAME_TIME_TICK_INTERVAL;

		if (car.state)
		{
			track.pos += car.speed;
			update_view();
			update_tree_view();
			task_post_pure_msg(NB_GAME_OBSTACLE_ID, NB_GAME_OBSTACLE_UPDATE);
		}
	}
	break;

	default:
		break;
	}
}

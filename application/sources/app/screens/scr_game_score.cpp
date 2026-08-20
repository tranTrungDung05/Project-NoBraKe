#include "scr_game_score.h"

static uint16_t best_scores[NB_GAME_SCORE_COUNT];

static uint16_t score_for_elapsed(uint16_t elapsed_s)
{
	uint16_t remaining = 0;
	if (track.time_limit_s > elapsed_s)
	{
		remaining = track.time_limit_s - elapsed_s;
	}

	uint32_t score = (1000U + ((uint32_t)remaining * 100U)) *
	                 ((uint32_t)nb_game_settings_get_difficulty() + 1U);
	return score > 0xFFFFU ? 0xFFFFU : (uint16_t)score;
}

uint16_t nb_game_score_get_current(void)
{
	return score_for_elapsed(nb_game_track_get_elapsed_s());
}

void nb_game_score_record_finish(uint16_t elapsed_s)
{
	uint16_t score = score_for_elapsed(elapsed_s);

	for (uint8_t i = 0; i < NB_GAME_SCORE_COUNT; ++i)
	{
		if (best_scores[i] != 0 && score <= best_scores[i])
		{
			continue;
		}

		for (uint8_t j = NB_GAME_SCORE_COUNT - 1; j > i; --j)
		{
			best_scores[j] = best_scores[j - 1];
		}
		best_scores[i] = score;
		break;
	}
}

static void view_scr_game_score()
{
	view_render.drawRoundRect(3, 3, 122, 58, 4, WHITE);
	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	view_render.setCursor(31, 6);
	view_render.print("HIGH SCORES");

	view_render.drawLine(10, 15, 118, 15, WHITE);
	view_render.setCursor(16, 18);
	view_render.print("RANK");
	view_render.setCursor(64, 18);
	view_render.print("SCORE");

	for (uint8_t i = 0; i < NB_GAME_SCORE_COUNT; ++i)
	{
		view_render.setCursor(18, 28 + (i * 8));
		view_render.print(i + 1);
		view_render.print(".");
		view_render.setCursor(68, 28 + (i * 8));
		if (best_scores[i] == 0)
		{
			view_render.print("--");
		}
		else
		{
			view_render.print(best_scores[i]);
		}
	}
}

view_dynamic_t dyn_view_item_game_score = {
    {.item_type = ITEM_TYPE_DYNAMIC},
    view_scr_game_score};

view_screen_t scr_game_score = {
    &dyn_view_item_game_score,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};

void scr_game_score_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case SCREEN_ENTRY:
		break;

	case AC_DISPLAY_BUTTON_MODE_PRESSED:
	case AC_DISPLAY_BUTTON_DOWN_PRESSED:
	{
		SCREEN_TRAN(scr_menu_game_handle, &scr_menu_game);
	}
	break;

	default:
		break;
	}
}

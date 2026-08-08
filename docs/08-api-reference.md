# API Reference

This is a small project API guide for people extending the game. It lists the
interfaces that are used by more than one source file. Private helper
functions inside a `.cpp` file are intentionally not listed.

## Car API

Header: `application/sources/app/game/nobrake_game/nb_game_car.h`

```cpp
extern nb_game_car_t car;
void nb_game_car_handle(ak_msg_t *msg);
```

Important constants:

| Name | Value | Meaning |
| --- | ---: | --- |
| `NB_GAME_CAR_W` | 24 | Player bitmap width |
| `NB_GAME_CAR_H` | 24 | Player bitmap height |
| `NB_GAME_SPEED_MAX` | 180 | Maximum speed value |
| `AXIS_X_CAR` | 55 | Starting x position |
| `AXIS_Y_CAR` | 40 | Starting y position |

## Track API

Header: `application/sources/app/game/nobrake_game/nb_game_track.h`

```cpp
extern nb_game_track_t track;
extern nb_game_line_t view_lines[NB_GAME_VISIBLE_LINES];
extern nb_game_tree_t view_trees[NB_GAME_TREE_COUNT];

void nb_game_track_handle(ak_msg_t *msg);
int16_t nb_game_track_get_lane_center_x(const nb_game_line_t *line,
                                        uint8_t lane_index);
uint8_t nb_game_track_get_random(uint8_t max);
uint16_t nb_game_track_get_elapsed_s(void);
uint8_t nb_game_track_get_time_left_s(void);
uint8_t nb_game_settings_get_difficulty(void);
uint8_t nb_game_settings_get_obstacle_max(void);
void nb_game_settings_change_difficulty(int8_t delta);
```

`view_lines` and `view_trees` are updated by the track task and read by the
screen and obstacle code.

## Obstacle API

Header: `application/sources/app/game/nobrake_game/nb_game_obstacle.h`

```cpp
void nb_game_obstacle_handle(ak_msg_t *msg);
uint8_t nb_game_obstacle_count(void);
const nb_game_obstacle_t *nb_game_obstacle_at(uint8_t index);
void nb_game_obstacle_update_world(void);
```

Obstacle type values are:

```cpp
NB_GAME_OBSTACLE_STONE
NB_GAME_OBSTACLE_BARRIER
NB_GAME_OBSTACLE_CAR
```

The obstacle array has a fixed capacity of `NB_GAME_OBSTACLE_COUNT`.

## Score API

Header: `application/sources/app/screens/scr_game_score.h`

```cpp
uint16_t nb_game_score_get_current(void);
void nb_game_score_record_finish(uint16_t elapsed_s);
```

The chart stores three scores in RAM. The score formula gives more points for
remaining time and applies a difficulty multiplier.

## Screen API

Each screen exports a view object and a message handler. Examples:

```cpp
extern view_screen_t scr_nobrake_game;
void scr_nobrake_game_handle(ak_msg_t *msg);
```

Screen transitions use the framework macro:

```cpp
SCREEN_TRAN(scr_game_finish_handle, &scr_game_finish);
```

The startup screen is part of the framework's screen list but is intentionally
not documented as part of the game implementation.

## Bitmap API

Header: `application/sources/app/screens/screens_bitmap.h`

All bitmap declarations are `PROGMEM` arrays. Definitions live in
`screens_bitmap.cpp`. Add a new bitmap in both places:

1. Define the byte array in `screens_bitmap.cpp`.
2. Add its `extern` declaration to `screens_bitmap.h`.
3. Include `screens_bitmap.h` in the screen that draws it.
4. Call `view_render.drawBitmap()` with the correct width and height.

## Buzzer API

Header: `application/sources/driver/buzzer/buzzer.h`

```cpp
void BUZZER_Init(void);
void BUZZER_Enable(uint16_t frequency, uint32_t duration);
void BUZZER_Disable(void);
void BUZZER_PlayTones(const Tone_TypeDef *tones);
```

The buzzer driver and game tone patterns live under
`application/sources/driver/buzzer/`.

## Build API

From the `application` directory:

```sh
make
```

The build uses `arm-none-eabi-g++`, `arm-none-eabi-gcc`, and the STM32L151
linker script. The application start address is configured as `0x08003000`
in the Makefile.

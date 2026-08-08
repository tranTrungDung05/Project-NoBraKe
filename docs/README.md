# NoBrake Documentation

NoBrake is a small event-driven driving game for the AK Embedded Base Kit
with an STM32L151 microcontroller and a 128x64 monochrome OLED.

## Reading Order

1. [Pseudo-3D theory](./01-pseudo3d-theory.md) explains projected road lines and the camera.
2. [Gameplay design](./02-gameplay-design.md) explains controls, difficulty, scoring, and screens.
3. [Framework architecture](./03-framework-architecture.md) explains tasks, signals, messages, and rendering ownership.
4. [Object sequence diagram](./04-object-sequence-diagram.md) shows communication between the game tasks.
5. [Signal execution flow](./05-signal-execution-flow.md) lists active signals and shows the game state machine.
6. [Rendering pipeline](./06-rendering-pipeline.md) explains how game data becomes OLED pixels.
7. [Road algorithm](./07-road-algorithm.md) describes projection, curves, trees, and obstacle placement.
8. [API reference](./08-api-reference.md) lists the interfaces used when extending the game.

## Source Map

| Area | Path |
| --- | --- |
| Signals and timers | `application/sources/app/app.h` |
| Task registration | `application/sources/app/task_list.cpp` |
| Car logic | `application/sources/app/game/nobrake_game/nb_game_car.cpp` |
| Track and projection | `application/sources/app/game/nobrake_game/nb_game_track.cpp` |
| Obstacle logic | `application/sources/app/game/nobrake_game/nb_game_obstacle.cpp` |
| Game screen and renderer | `application/sources/app/screens/scr_nobrake_game.cpp` |
| Other screens | `application/sources/app/screens/` |
| Bitmap assets | `application/sources/app/screens/screens_bitmap.cpp` |
| Buzzer driver and tones | `application/sources/driver/buzzer/` |

## Build

From the repository root:

```sh
cd application
make
```

The local Makefile expects the ARM GCC toolchain path configured by
`GCC_PATH`. See [framework architecture](./03-framework-architecture.md) for
the build details.

# Framework Architecture

The project runs on an event-driven embedded framework. Code is organized as
tasks that receive messages. A message contains a signal, and the task's
handler decides what to do for that signal.

## The Four Basic Ideas

### Task

A task is a registered message handler. The NoBrake tasks are registered in
`application/sources/app/task_list.cpp`:

| Task ID | Handler | Responsibility |
| --- | --- | --- |
| `NB_GAME_TRACK_ID` | `nb_game_track_handle` | Road projection, movement, trees, difficulty data |
| `NB_GAME_OBSTACLE_ID` | `nb_game_obstacle_handle` | Obstacle ring buffer and projected obstacle positions |
| `NB_GAME_CAR_ID` | `nb_game_car_handle` | Speed, steering, road collision, obstacle collision |
| `NB_GAME_SCREEN_ID` | `scr_nobrake_game_handle` | Input, game state, timers, and screen rendering |

The display task is also registered in the same table. It dispatches screen
messages through the screen manager.

### Signal

A signal is an event name such as `NB_GAME_CAR_UPDATE` or
`AC_DISPLAY_BUTTON_MODE_PRESSED`. Signal definitions are in
`application/sources/app/app.h`.

### Message

A message carries a signal to a task. A simple event-driven message looks
like this:

```cpp
task_post_pure_msg(NB_GAME_CAR_ID, NB_GAME_CAR_UPDATE);
```

The framework places the message in the destination task's queue. The task
later calls its handler with that message.

### Handler

Each task has a central handler with a `switch` statement:

```cpp
void nb_game_car_handle(ak_msg_t *msg) {
    switch (msg->sig) {
    case NB_GAME_CAR_UPDATE:
        // Update the car.
        break;
    default:
        break;
    }
}
```

This makes it possible to find the code for a signal by searching for its
handler and its `case` label.

## Screen Manager

The screen manager stores the current screen handler and its view object.
When the display task receives a message, it:

1. Dispatches the message to the current screen handler.
2. Lets the handler change screen state or post game messages.
3. Renders the current screen view.

Screen transitions call the new screen with `SCREEN_ENTRY`, render it, and
later deliver `SCREEN_EXIT` when the screen changes again.

The game screen is a dynamic view. Its renderer reads shared game state such
as `car`, `track`, `view_lines`, `view_trees`, and the projected obstacles.
Game tasks update data; the screen task draws that data.

## Event-Driven Race Update

The game screen starts a 70 ms periodic timer. While the car is moving, the
normal update chain is:

```text
Display timer
    -> Game screen: NB_GAME_TIME_TICK
    -> Car: NB_GAME_CAR_UPDATE
    -> Track: NB_GAME_TRACK_UPDATE
    -> Obstacle: NB_GAME_OBSTACLE_UPDATE
    -> Car: NB_GAME_CAR_CHECK_CRASH
```

The screen posts steering before the car update when a steering button is
held. The car changes speed and position, then asks the track to update.
The track projects the road and trees and asks the obstacle task to project
obstacles. The obstacle task asks the car to check collision last.

When the car is stopped, the screen posts `NB_GAME_TRACK_UPDATE` directly.
The track advances elapsed race time but does not move the road or update
obstacles. This keeps the HUD clock independent from road movement without
creating a second moving-world update chain.

## Data Ownership

The project uses simple shared C/C++ structs rather than a large game engine:

| Data | Main owner | Used by |
| --- | --- | --- |
| `car` | Car task | Track, screen, collision code |
| `track` | Track task | Car, score, screen |
| `view_lines` | Track task | Obstacles and screen |
| `view_trees` | Track task | Screen |
| `nb_game_obstacles` | Obstacle task | Car and screen |
| Bitmap arrays | `screens_bitmap.cpp` | Screen files |

The important separation is that game tasks update data and screen files
render it. A task other than the screen does not draw directly to the OLED.

## Source Layout

```text
application/
  Makefile
  sources/
    app/
      app.h                 Signals and timer values
      task_list.cpp         Task registration
      game/nobrake_game/    Car, track, and obstacle tasks
      screens/              Screen handlers and drawing
    common/                 Framework screen and drawing support
    driver/buzzer/          Buzzer driver and tone patterns
```

## Build

From the `application` directory:

```sh
make
```

The Makefile creates an application image under
`application/build_ak-base-kit-stm32l151-application/`. The local Makefile
expects the ARM GCC toolchain at the path stored in `GCC_PATH`; change that
variable when using a different machine.

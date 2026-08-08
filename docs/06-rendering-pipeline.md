# Rendering Pipeline

The display is a 128x64 monochrome OLED. The game uses the framework's
`view_screen_t` and `view_dynamic_t` objects instead of drawing from the
car, track, or obstacle task.

## From Message to Pixels

```text
Timer or button message
        |
        v
task_display()
        |
        v
screen_manager dispatch
        |
        v
scr_nobrake_game_handle()
        |
        v
screen manager renders dyn_view_item_nobrake_game
        |
        v
draw_game()
        |
        v
OLED framebuffer / display driver
```

The screen manager is implemented in
`application/sources/common/screen_manager.cpp`. The display task dispatches
messages to the current screen and then renders the current screen view.

## Game Draw Order

`draw_game()` renders the active race in this order:

1. Sky and moving cloud bitmaps.
2. Projected road edges, rumble strips, and lane markers.
3. Projected trees outside the road.
4. Projected stone, barrier, or mini-car obstacles.
5. The player's car bitmap.
6. Crash outline when the car is in a crash state.
7. HUD time and speed.

During the finishing state, the finish gate is drawn before the car and the
HUD remains visible while the car brakes.

## Dynamic View

The game screen exposes:

```cpp
view_dynamic_t dyn_view_item_nobrake_game;
view_screen_t scr_nobrake_game;
```

The dynamic view points to `draw_game`. The view reads the current values of
the shared game structs. It does not own the car position or road position.

## Bitmap Ownership

All game bitmap arrays are defined in
`application/sources/app/screens/screens_bitmap.cpp` and declared in
`application/sources/app/screens/screens_bitmap.h`. Current assets include:

- Player car straight view.
- Welcome-screen side car.
- Large tree.
- Cloud.
- Menu icons.
- Stone, barrier, and mini-car obstacles.

The screen code only selects an asset and calls `drawBitmap`. This keeps raw
bitmap data out of the screen handlers.

## Performance Notes

- The screen renderer uses integer arithmetic.
- The track keeps a fixed array of 24 projected lines.
- Trees and obstacles use fixed-size ring buffers.
- Game tasks do not draw directly.
- The display task has a minimum render interval configured by the framework.

The renderer is still work done by the display task, so expensive new
drawing loops should be avoided. A new animation should use a timer and a
small frame counter rather than posting many messages per pixel.

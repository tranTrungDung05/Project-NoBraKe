# Road Algorithm

This document describes the implementation in
`application/sources/app/game/nobrake_game/nb_game_track.cpp`.

## World Values

The world is measured in integer units. The track starts at `track.pos = 0`
and ends at a difficulty dependent length. The road is split into segments
of `NB_GAME_SEGMENT_LENGTH` units.

The renderer keeps 24 visible lines. A line is created from its distance from
the camera and from the accumulated curve direction.

## Projecting a Line

The current code follows this simplified process:

1. Find the segment containing `track.pos`.
2. Find the camera offset inside that segment.
3. For each visible line, calculate its relative depth `dz`.
4. Calculate an integer projection scale.
5. Convert world x and y into screen `X` and `Y`.
6. Convert world road width into screen `W`.
7. Mark lines hidden by a closer line.

Conceptually:

```text
dz    = line distance - camera offset
scale = camera depth * 256 / dz
X     = screen center + projected road x
Y     = screen center - projected camera height
W     = projected road width
```

The `256` factor provides fixed-point precision without floating-point math.

## Curved Road

The track uses a 16-entry signed curve lookup table. Each projected line
uses one table entry multiplied by `curve_lut_gain`. The code accumulates a
road direction and road x position:

```text
road_x  starts at 0
road_dx starts at 0

for each line:
    line.curve = curve_lut[current] * gain
    line.x = road_x
    road_x = road_x + road_dx
    road_dx = road_dx + line.curve
```

The lookup index advances every seven road lines and wraps around the table.
This is intentionally small and deterministic for the microcontroller.

## Camera Centering

The camera x position is calculated from the center of the car:

```text
camera_x = car.x + car_width / 2 - screen_width / 2
```

The track converts this screen-space camera value into a world-space offset
before projecting every line. As a result, the road appears to move under a
car that stays near the center of the display.

## Road Collision Area

After projection, the track chooses the line whose screen y coordinate is
closest to the car. It stores that line's center and width in:

- `track.player_center_x`
- `track.player_half_width`

The car task uses these values to decide whether the car center is outside the
road. The car task also checks the projected obstacle rectangles.

## Tree Ring Buffer

Trees are stored in a six-entry ring buffer. Every tree has:

- A world `z` position.
- A side: left or right of the road.
- A projected line index.
- A screen x and y position.
- A size based on depth.

When a tree passes the camera, the head is removed and a new tree is pushed at
the tail. This keeps memory usage fixed and avoids allocating tree objects
during gameplay.

## Obstacle Projection

Obstacles use the same line projection data. Each obstacle stores a world
position, lane, type, and projected screen position. The track supplies the
lane center. The obstacle task removes old entries, fills missing entries, and
updates the projected coordinates.

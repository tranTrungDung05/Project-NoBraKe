# Object Sequence

This file show how the game objects talk to each other. The project does not
use one big game loop. Each task receives a message and does one small job.

The main objects in the game are:

- `Car`: speed, steering, and collision.
- `Track`: road projection, road movement, and trees.
- `Obstacle`: obstacle positions and obstacle projection.
- `Game screen`: buttons, timer, and drawing.

## 1. Car Sequence Logic

The button handler only saves the steering direction. After that, the 70 ms
timer event sends the work to the scheduler, then the car task receives it.

```mermaid
%%{init: {'theme':'base','themeVariables':{'fontSize':'18px','primaryColor':'#1565c0','primaryTextColor':'#ffffff','primaryBorderColor':'#0d47a1','lineColor':'#90a4ae','signalColor':'#ffc107','signalTextColor':'#ffc107','actorBkg':'#1565c0','actorBorder':'#0d47a1','actorTextColor':'#ffffff','actorLineColor':'#90caf9','noteBkgColor':'#fff59d','noteTextColor':'#000000','noteBorderColor':'#f57f17','activationBkgColor':'#66bb6a','activationBorderColor':'#2e7d32','sequenceNumberColor':'#ffffff','loopTextColor':'#ffc107','labelBoxBkgColor':'#37474f','labelBoxBorderColor':'#90a4ae','labelTextColor':'#ffffff'},'sequence':{'actorMargin':120,'messageFontSize':17,'noteFontSize':15,'actorFontSize':17,'boxMargin':15,'boxTextMargin':8,'noteMargin':12,'useMaxWidth':false}}}%%
sequenceDiagram
    autonumber
    actor Btn as Button
    participant Scr as Screen task
    participant Q as AKOS Scheduler
    participant Car as Car task
    participant Trk as Track task

    Note over Btn,Scr: Button event can come between two timer ticks
    Btn-)Scr: AC_DISPLAY_BUTTON_UP_PRESSED
    activate Scr
    Note right of Scr: steer_hold = RIGHT
    deactivate Scr
    Btn-)Scr: AC_DISPLAY_BUTTON_DOWN_PRESSED
    activate Scr
    Note right of Scr: steer_hold = LEFT
    deactivate Scr
    Btn-)Scr: AC_DISPLAY_BUTTON_*_RELEASED
    activate Scr
    Note right of Scr: release the same direction<br/>steer_hold = NONE
    deactivate Scr

    loop Each NB_GAME_TIME_TICK while car is running
        alt steer_hold == RIGHT
            Scr-)Q: NB_GAME_CAR_TURN_RIGHT
            Q-)Car: NB_GAME_CAR_TURN_RIGHT
            activate Car
            Note right of Car: car.x += steer_step
            deactivate Car
        else steer_hold == LEFT
            Scr-)Q: NB_GAME_CAR_TURN_LEFT
            Q-)Car: NB_GAME_CAR_TURN_LEFT
            activate Car
            Note right of Car: car.x -= steer_step
            deactivate Car
        end
        Scr-)Q: NB_GAME_CAR_UPDATE
        Q-)Car: NB_GAME_CAR_UPDATE
        activate Car
        Note right of Car: update speed<br/>send NB_GAME_TRACK_UPDATE
        Car-)Q: NB_GAME_TRACK_UPDATE
        deactivate Car
    end
```

<p align="center"><strong><em>Figure 1:</em></strong> How the car object works</p>

When MODE is held, the screen sends the throttle message. When MODE is
released, the car slowly goes down speed.

## 2. Road Sequence

The screen starts the update by sending a message to the car. The car then
asks the track to update the road, trees, and game time.

```mermaid
%%{init: {'theme':'base','themeVariables':{'fontSize':'18px','primaryColor':'#1565c0','primaryTextColor':'#ffffff','primaryBorderColor':'#0d47a1','lineColor':'#90a4ae','signalColor':'#ffc107','signalTextColor':'#ffc107','actorBkg':'#1565c0','actorBorder':'#0d47a1','actorTextColor':'#ffffff','actorLineColor':'#90caf9','noteBkgColor':'#fff59d','noteTextColor':'#000000','noteBorderColor':'#f57f17','activationBkgColor':'#66bb6a','activationBorderColor':'#2e7d32','sequenceNumberColor':'#ffffff','loopTextColor':'#ffc107','labelBoxBkgColor':'#37474f','labelBoxBorderColor':'#90a4ae','labelTextColor':'#ffffff'},'sequence':{'actorMargin':120,'messageFontSize':17,'noteFontSize':15,'actorFontSize':17,'boxMargin':15,'boxTextMargin':8,'noteMargin':12,'useMaxWidth':false}}}%%
sequenceDiagram
    autonumber
    participant Scr as Screen task
    participant Q as AKOS Scheduler
    participant Car as Car task
    participant Trk as Track task

    Note over Scr: NB_GAME_TIME_TICK
    Scr-)Q: NB_GAME_CAR_UPDATE
    Q-)Car: NB_GAME_CAR_UPDATE
    activate Car
    Note right of Car: update speed<br/>send track update
    Car-)Q: NB_GAME_TRACK_UPDATE
    deactivate Car

    Q-)Trk: NB_GAME_TRACK_UPDATE
    activate Trk
    Note right of Trk: track.pos += car.speed<br/>elapsed_ms += 70
    Trk->>Trk: Calculate road lines and trees
    Note right of Trk: if speed = 0, only add time
    deactivate Trk
```

<p align="center"><strong><em>Figure 2:</em></strong> How the road object works</p>

## 3. Obstacle Sequence

After the track is updated, it sends a message to the obstacle task. The
obstacle task calculates its new position and asks the car to check hitbox.

```mermaid
%%{init: {'theme':'base','themeVariables':{'fontSize':'18px','primaryColor':'#1565c0','primaryTextColor':'#ffffff','primaryBorderColor':'#0d47a1','lineColor':'#90a4ae','signalColor':'#ffc107','signalTextColor':'#ffc107','actorBkg':'#1565c0','actorBorder':'#0d47a1','actorTextColor':'#ffffff','actorLineColor':'#90caf9','noteBkgColor':'#fff59d','noteTextColor':'#000000','noteBorderColor':'#f57f17','activationBkgColor':'#66bb6a','activationBorderColor':'#2e7d32','sequenceNumberColor':'#ffffff','loopTextColor':'#ffc107','labelBoxBkgColor':'#37474f','labelBoxBorderColor':'#90a4ae','labelTextColor':'#ffffff'},'sequence':{'actorMargin':120,'messageFontSize':17,'noteFontSize':15,'actorFontSize':17,'boxMargin':15,'boxTextMargin':8,'noteMargin':12,'useMaxWidth':false}}}%%
sequenceDiagram
    autonumber
    participant Scr as Screen task
    participant Q as AKOS Scheduler
    participant Car as Car task
    participant Trk as Track task
    participant Obs as Obstacle task

    Scr-)Q: NB_GAME_CAR_UPDATE
    Q-)Car: NB_GAME_CAR_UPDATE
    activate Car
    Car-)Q: NB_GAME_TRACK_UPDATE
    deactivate Car

    Q-)Trk: NB_GAME_TRACK_UPDATE
    activate Trk
    Note right of Trk: update road and project track objects
    Trk-)Q: NB_GAME_OBSTACLE_UPDATE
    deactivate Trk

    Q-)Obs: NB_GAME_OBSTACLE_UPDATE
    activate Obs
    Note right of Obs: clear old view<br/>calculate lane and bitmap position
    Obs-)Q: NB_GAME_CAR_CHECK_CRASH
    deactivate Obs

    Q-)Car: NB_GAME_CAR_CHECK_CRASH
    activate Car
    Note right of Car: check road and front part hitbox
    deactivate Car
```

<p align="center"><strong><em>Figure 3:</em></strong> How the obstacle object works</p>

The car checks collision after the obstacle task finishes, so it checks the
new obstacle position instead of the old position.

## When the Car Stops

When the car stops, the road should not scroll. But the time still needs to
run, so the screen sends the track update directly. The road and obstacle stay
in the same place.

```text
Game screen -> Track: NB_GAME_TRACK_UPDATE
Track -> Track: Add time only
Track: Do not move road and obstacles
```

## Simple Summary

```text
Button input
    -> Game screen
    -> Car
    -> Track
    -> Obstacle
    -> Car collision check
    -> Screen draw
```

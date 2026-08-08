# Screen Sequence

This file show what the game screen does in three important moments:

1. Game start.
2. Game play.
3. Game reset.

The screen handler code is in
`application/sources/app/screens/scr_nobrake_game.cpp`.

## 1. Game Start

When the game screen opens, it sends setup messages to the car, track, and
obstacle. It also starts the game timer with 70 ms period.

```mermaid
%%{init: {'theme':'base','themeVariables':{'fontSize':'18px','primaryColor':'#1565c0','primaryTextColor':'#ffffff','primaryBorderColor':'#0d47a1','lineColor':'#90a4ae','signalColor':'#ffc107','signalTextColor':'#ffc107','actorBkg':'#1565c0','actorBorder':'#0d47a1','actorTextColor':'#ffffff','actorLineColor':'#90caf9','noteBkgColor':'#fff59d','noteTextColor':'#000000','noteBorderColor':'#f57f17','activationBkgColor':'#66bb6a','activationBorderColor':'#2e7d32','sequenceNumberColor':'#ffffff','loopTextColor':'#ffc107','labelBoxBkgColor':'#37474f','labelBoxBorderColor':'#90a4ae','labelTextColor':'#ffffff'},'sequence':{'actorMargin':120,'messageFontSize':17,'noteFontSize':15,'actorFontSize':17,'boxMargin':15,'boxTextMargin':8,'noteMargin':12,'useMaxWidth':false}}}%%
sequenceDiagram
    autonumber
    participant Menu
    participant Scr as Screen task
    participant Q as AKOS Scheduler
    participant Car as Car task
    participant Trk as Track task
    participant Obs as Obstacle task
    participant Tmr as Timer

    Menu-)Scr: Open Play
    activate Scr
    Note over Scr: SCREEN_ENTRY
    Scr-)Q: NB_GAME_CAR_SETUP
    Scr-)Q: NB_GAME_TRACK_SETUP
    Scr-)Q: NB_GAME_OBSTACLE_SETUP
    Note right of Scr: set game_state = GAME_PLAY
    Scr->>Tmr: timer_set(NB_GAME_TIME_TICK, 70 ms, PERIODIC)
    deactivate Scr

    Q-)Car: NB_GAME_CAR_SETUP
    activate Car
    Note right of Car: put car at the start position
    deactivate Car

    Q-)Trk: NB_GAME_TRACK_SETUP
    activate Trk
    Note right of Trk: create road lines<br/>and tree ring buffer
    deactivate Trk

    Q-)Obs: NB_GAME_OBSTACLE_SETUP
    activate Obs
    Note right of Obs: fill obstacle ring buffer with obstacles
    deactivate Obs
```

<p align="center"><strong><em>Figure 1:</em></strong> Game start sequence logic</p>

## 2. Game Play

The timer event is the main event of the game. When the car is moving, the
screen starts the chain car -> track -> obstacle.

```mermaid
%%{init: {'theme':'base','themeVariables':{'fontSize':'18px','primaryColor':'#1565c0','primaryTextColor':'#ffffff','primaryBorderColor':'#0d47a1','lineColor':'#90a4ae','signalColor':'#ffc107','signalTextColor':'#ffc107','actorBkg':'#1565c0','actorBorder':'#0d47a1','actorTextColor':'#ffffff','actorLineColor':'#90caf9','noteBkgColor':'#fff59d','noteTextColor':'#000000','noteBorderColor':'#f57f17','activationBkgColor':'#66bb6a','activationBorderColor':'#2e7d32','sequenceNumberColor':'#ffffff','loopTextColor':'#ffc107','labelBoxBkgColor':'#37474f','labelBoxBorderColor':'#90a4ae','labelTextColor':'#ffffff'},'sequence':{'actorMargin':120,'messageFontSize':17,'noteFontSize':15,'actorFontSize':17,'boxMargin':15,'boxTextMargin':8,'noteMargin':12,'useMaxWidth':false}}}%%
sequenceDiagram
    autonumber
    participant Tmr as Timer
    participant Scr as Screen task
    participant Q as AKOS Scheduler
    participant Car as Car task
    participant Trk as Track task
    participant Obs as Obstacle task
    participant Oled as OLED renderer

    loop Every 70 ms while car is moving
        Tmr-)Scr: NB_GAME_TIME_TICK
        activate Scr
        Note right of Scr: read button and update HUD
        Scr-)Q: NB_GAME_CAR_UPDATE
        deactivate Scr

        Q-)Car: NB_GAME_CAR_UPDATE
        activate Car
        Note right of Car: update speed and car.x
        Car-)Q: NB_GAME_TRACK_UPDATE
        deactivate Car

        Q-)Trk: NB_GAME_TRACK_UPDATE
        activate Trk
        Note right of Trk: move road and add game time
        Trk-)Q: NB_GAME_OBSTACLE_UPDATE
        deactivate Trk

        Q-)Obs: NB_GAME_OBSTACLE_UPDATE
        activate Obs
        Note right of Obs: calculate active obstacle view
        Obs-)Q: NB_GAME_CAR_CHECK_CRASH
        deactivate Obs

        Q-)Car: NB_GAME_CAR_CHECK_CRASH
        activate Car
        Note right of Car: check road and front part hitbox
        deactivate Car

        Scr->>Oled: draw current game data
    end
```

<p align="center"><strong><em>Figure 2:</em></strong> Game play sequence logic</p>

When the car is stopped, the screen sends `NB_GAME_TRACK_UPDATE` directly.
This keeps the time running while the road and obstacles stay still. If the
time limit is reached, the game goes to game over even when the car is stopped.

## 3. Game Reset

When the game screen closes, it removes the timer and clears the objects for
the next race.

```mermaid
%%{init: {'theme':'base','themeVariables':{'fontSize':'18px','primaryColor':'#1565c0','primaryTextColor':'#ffffff','primaryBorderColor':'#0d47a1','lineColor':'#90a4ae','signalColor':'#ffc107','signalTextColor':'#ffc107','actorBkg':'#1565c0','actorBorder':'#0d47a1','actorTextColor':'#ffffff','actorLineColor':'#90caf9','noteBkgColor':'#fff59d','noteTextColor':'#000000','noteBorderColor':'#f57f17','activationBkgColor':'#66bb6a','activationBorderColor':'#2e7d32','sequenceNumberColor':'#ffffff','loopTextColor':'#ffc107','labelBoxBkgColor':'#37474f','labelBoxBorderColor':'#90a4ae','labelTextColor':'#ffffff'},'sequence':{'actorMargin':120,'messageFontSize':17,'noteFontSize':15,'actorFontSize':17,'boxMargin':15,'boxTextMargin':8,'noteMargin':12,'useMaxWidth':false}}}%%
sequenceDiagram
    autonumber
    participant Next as Next screen
    participant Scr as Screen task
    participant Tmr as Timer
    participant Q as AKOS Scheduler
    participant Car as Car task
    participant Obs as Obstacle task

    Next-)Scr: SCREEN_EXIT
    activate Scr
    Scr->>Tmr: timer_remove_attr(NB_GAME_TIME_TICK)
    Scr-)Q: NB_GAME_CAR_RESET
    Scr-)Q: NB_GAME_OBSTACLE_RESET
    deactivate Scr

    Q-)Car: NB_GAME_CAR_RESET
    activate Car
    Note right of Car: speed = 0<br/>throttle = 0<br/>hide car
    deactivate Car

    Q-)Obs: NB_GAME_OBSTACLE_RESET
    activate Obs
    Note right of Obs: clear obstacle ring buffer
    deactivate Obs
```

<p align="center"><strong><em>Figure 3:</em></strong> Game reset sequence logic</p>

The track does not have a reset message now. It is initialized again by
`NB_GAME_TRACK_SETUP` when the next race starts.

## Main Signals

| Signal | Used by | Meaning |
| --- | --- | --- |
| `NB_GAME_TIME_TICK` | Game screen | Main 70 ms game event |
| `NB_GAME_CAR_UPDATE` | Car | Change speed and notify track |
| `NB_GAME_TRACK_UPDATE` | Track | Update time and, if moving, the road |
| `NB_GAME_OBSTACLE_UPDATE` | Obstacle | Update obstacle positions |
| `NB_GAME_CAR_CHECK_CRASH` | Car | Check collision |
| `NB_GAME_CAR_RESET` | Car | Stop and hide the car |
| `NB_GAME_OBSTACLE_RESET` | Obstacle | Clear obstacles |

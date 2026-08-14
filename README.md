<h1 align="center">No Brake - Game built on AK Embedded Base Kit</h1>

<table align="center">
  <tr>
    <td align="center">
      <img
        src="hardware/images/screen/nb_banner_night.gif"
        alt="No Brake finish"
        width="720"
      />
    </td>
  </tr>
</table>

<h2 align="center">Gameplay Demo</h2>

<div align="center">
  <video
    src="https://github.com/user-attachments/assets/119c1d49-5e34-485f-9d85-bea0a484c2a3"
    controls
    width="720"
  ></video>
</div>
<h2 align="center">Documentation</h2>

| File | Description |
|---|---|
| [README.md](README.md) | Main project overview, hardware information, gameplay rules, and object descriptions. |
| [01-pseudo3d-theory.md](docs/01-pseudo3d-theory.md) | Explains projected road lines and the camera. |
| [02-gameplay-design.md](docs/02-gameplay-design.md) | Explains controls, difficulty, scoring, and screens. |
| [03-framework-architecture.md](docs/03-framework-architecture.md) | Explains tasks, signals, messages, and rendering ownership. |
| [04-object-sequence-diagram.md](docs/04-object-sequence-diagram.md) | Shows communication between the game tasks. |
| [05-signal-execution-flow.md](docs/05-signal-execution-flow.md) | Lists active signals and shows the game state machine. |
| [06-rendering-pipeline.md](docs/06-rendering-pipeline.md) | Explains how game data becomes OLED pixels. |
| [07-api-reference.md](docs/07-api-reference.md) | Lists the interfaces used when extending the game. |

<h2 align="center">Introduction</h2>

No Brake is a small event-driven driving game for the AK Embedded Base Kit with an STM32L151 microcontroller and a 128x64 monochrome OLED.


<h3 align="center">I. Hardware</h3>

<p align="center">
  <a href="https://epcb.vn/products/ak-embedded-base-kit-lap-trinh-nhung-vi-dieu-khien-mcu"><img src="hardware/images/board/ak-embedded-base-kit-version-3.jpg" width="480"/></a>
</p>


[AK Embedded Base Kit](https://epcb.vn/products/ak-embedded-base-kit-lap-trinh-nhung-vi-dieu-khien-mcu) is an evaluation kit aimed at intermediate and advanced embedded software learners. *You can learn more in the following link:*  [ak-base-kit-stm32l151](https://github.com/the-ak-foundation/ak-base-kit-stm32l151)

<h3 align="center">II. How to Play</h3>
<table align="center">
  <tr>
    <td align="center"><img src="hardware/images/screen/nb_game_welcome.gif" alt="No Brake welcome" width="480"/></td>
  </tr>
</table>
<p align="center"><strong><em>Figure 1:</em></strong> Welcome screen</p>

The game opens on the **Menu**, which offers the following options:

- **Play:** Start to play.
- **Setting:** Configure gameplay difficulty.
- **Charts:** View the top 3 highest scores.
- **Exit:** Leave to the idle screen.

<table align="center">
  <tr>
    <td align="center"><img src="hardware/images/screen/nb_game_play.png" alt="No Brake gameplay" width="480"/></td>
  </tr>
</table>
<p align="center"><strong><em>Figure 2:</em></strong> Gameplay screen</p>


The board provides three buttons:

| Button | In the menu | During the race |
| --- | --- | --- |
| `UP` | Move selection up | Steer right |
| `DOWN` | Move selection down | Steer left |
| `MODE` | Select an item | Hold to accelerate |

Releasing `UP` or `DOWN` stops steering. Releasing `MODE` stops
acceleration. The car has no brake button. It gradually slows down when the
accelerator is released.

The steering step becomes smaller at high speed. This makes fast driving more
risky without adding a separate brake system.

The player loses when:

- The car leaves the road boundaries.
- The car overlaps an obstacle in the collision area.
- The time limit reaches zero before the finish.

The player wins when reaching the track length. The car then uses the finish-brake sequence before the finish screen opens.

<table align="center">
  <tr>
    <td align="center"><img src="hardware/images/screen/nb_game_finish.gif" alt="No Brake finish" width="480"/></td>
  </tr>
</table>
<p align="center"><strong><em>Figure 3:</em></strong> Finish screen</p>

Difficulty is changed from the Settings screen with `UP` and `DOWN`.

| Difficulty | Time limit | Track length | Maximum active obstacles |
| --- | ---: | ---: | ---: |
| Easy | 90 seconds | 75% | 1 |
| Normal | 70 seconds | 75% | 1 |
| Hard | 60 seconds | 100% | 2 |

The car starts at the center of the road. Holding `MODE` increases speed up
to the configured maximum. Releasing it makes the car coast down. Steering is
possible only while the car has a positive speed.

<table align="center">
  <tr>
    <td align="center"><img src="hardware/images/screen/nb_game_score.png" alt="No Brake score" width="480"/></td>
  </tr>
</table>
<p align="center"><strong><em>Figure 4:</em></strong> Score screen</p>

The score is based on how quickly you finish the race. The more time you have left, the higher your score. Higher difficulty levels also apply a multiplier to the final score.

<h3 align="center">III. Game sequence diagram</h3>

```mermaid
%%{init: {
  "theme": "base",
  "themeVariables": {
    "fontSize": "20px",

    "primaryColor": "#263B5A",
    "primaryTextColor": "#F5F7FA",
    "primaryBorderColor": "#5B789E",

    "lineColor": "#78909C",
    "textColor": "#E3E8EF",

    "signalColor": "#FFD180",
    "signalTextColor": "#FFE0B2",

    "actorBkg": "#344767",
    "actorBorder": "#607D9F",
    "actorTextColor": "#F5F7FA",
    "actorLineColor": "#78909C",

    "noteBkgColor": "#4A4A42",
    "noteTextColor": "#FFF8E1",
    "noteBorderColor": "#8D846B",

    "activationBkgColor": "#3E6658",
    "activationBorderColor": "#668F7E",

    "sequenceNumberColor": "#F5F7FA",

    "loopTextColor": "#B0BEC5",

    "labelBoxBkgColor": "#394B63",
    "labelBoxBorderColor": "#607D9F",
    "labelTextColor": "#F5F7FA"
  },

  "sequence": {
    "actorMargin": 55,
    "noteMargin": 12,
    "messageAlign": "center",
    "messageFontSize": 19,
    "noteFontSize": 17,
    "actorFontSize": 20
  }
}}%%
sequenceDiagram
    autonumber

    participant Player
    participant Scr as Screen task
    participant Q as AKOS Scheduler
    participant Tmr as Timer
    participant Car as Car task
    participant Trk as Track task
    participant Obs as Obstacle task
    participant Next as Next screen

    Note over Player,Scr: GAME_START

    Player->>Scr: Open Play
    activate Scr

    Scr->>Q: NB_GAME_CAR_SETUP
    Scr->>Q: NB_GAME_TRACK_SETUP
    Scr->>Q: NB_GAME_OBSTACLE_SETUP
    Scr->>Scr: game_state = GAME_PLAY
    Scr->>Tmr: timer_set(NB_GAME_TIME_TICK, 70 ms, PERIODIC)

    deactivate Scr

    Q->>Car: NB_GAME_CAR_SETUP
    activate Car
    Note right of Car: Put car at start position
    deactivate Car

    Q->>Trk: NB_GAME_TRACK_SETUP
    activate Trk
    Note right of Trk: Create road lines<br/>and tree ring buffer
    deactivate Trk

    Q->>Obs: NB_GAME_OBSTACLE_SETUP
    activate Obs
    Note right of Obs: Fill obstacle ring buffer
    deactivate Obs

    Note over Player,Scr: GAME_PLAY

    loop Every 70 ms

        Tmr->>Scr: NB_GAME_TIME_TICK
        activate Scr

        Scr->>Scr: Read button input<br/>and update HUD
        Scr->>Q: NB_GAME_CAR_UPDATE

        deactivate Scr

        alt Car is moving

            Q->>Car: NB_GAME_CAR_UPDATE
            activate Car
            Note right of Car: Update speed and car.x
            Car->>Q: NB_GAME_TRACK_UPDATE
            deactivate Car

            Q->>Trk: NB_GAME_TRACK_UPDATE
            activate Trk
            Note right of Trk: Move road<br/>and add game time
            Trk->>Q: NB_GAME_OBSTACLE_UPDATE
            deactivate Trk

            Q->>Obs: NB_GAME_OBSTACLE_UPDATE
            activate Obs
            Note right of Obs: Calculate active obstacle view
            Obs->>Q: NB_GAME_CAR_CHECK_CRASH
            deactivate Obs

            Q->>Car: NB_GAME_CAR_CHECK_CRASH
            activate Car
            Note right of Car: Check road and<br/>front-part hitbox
            deactivate Car

        else Car is stopped

            Scr->>Q: NB_GAME_TRACK_UPDATE

            Q->>Trk: NB_GAME_TRACK_UPDATE
            activate Trk
            Note right of Trk: Add game time only<br/>Do not move road or obstacles
            Trk->>Q: NB_GAME_OBSTACLE_UPDATE
            deactivate Trk

            Q->>Obs: NB_GAME_OBSTACLE_UPDATE
            activate Obs
            Note right of Obs: Keep obstacle position
            deactivate Obs

        end

    end

    Note over Player,Scr: GAME_RESET

    Next->>Scr: SCREEN_EXIT
    activate Scr

    Scr->>Tmr: timer_remove_attr(NB_GAME_TIME_TICK)
    Scr->>Q: NB_GAME_CAR_RESET
    Scr->>Q: NB_GAME_OBSTACLE_RESET

    deactivate Scr

    Q->>Car: NB_GAME_CAR_RESET
    activate Car
    Note right of Car: speed = 0<br/>throttle = 0<br/>hide car
    deactivate Car

    Q->>Obs: NB_GAME_OBSTACLE_RESET
    activate Obs
    Note right of Obs: Clear obstacle ring buffer
    deactivate Obs
```
<p align="center"><strong><em>Figure 5:</em></strong> Basic game sequences </p>

## Contact & Support

<p style="font-size: 18px;"><strong>Tran Trung Dung</strong> - Software Engineer</p>

<a href="https://github.com/tranTrungDung05">
  <img src="https://img.shields.io/badge/GitHub-tranTrungDung05-181717?style=for-the-badge&logo=github&logoColor=white"/>
</a>

<a href="https://www.linkedin.com/in/tran-trung-dung">
  <img src="https://img.shields.io/badge/LinkedIn-tran--trung--dung-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white"/>
</a>

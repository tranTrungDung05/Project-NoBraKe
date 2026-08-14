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

<table align="center" border="1" cellpadding="8" cellspacing="0" style="border-collapse: collapse; width:80%;">
  <tr>
    <th>File</th>
    <th>Description</th>
  </tr>
  <tr>
    <td><a href="README.md">README.md</a></td>
    <td>Main project overview, hardware information, gameplay rules, and object descriptions.</td>
  </tr>
  <tr>
    <td><a href="docs/01-pseudo3d-theory.md">01-pseudo3d-theory.md</a></td>
    <td>Explains projected road lines and the camera.</td>
  </tr>
  <tr>
    <td><a href="docs/02-gameplay-design.md">02-gameplay-design.md</a></td>
    <td>Explains controls, difficulty, scoring, and screens.</td>
  </tr>
  <tr>
    <td><a href="docs/03-framework-architecture.md">03-framework-architecture.md</a></td>
    <td>Explains tasks, signals, messages, and rendering ownership.</td>
  </tr>
  <tr>
    <td><a href="docs/04-object-sequence-diagram.md">04-object-sequence-diagram.md</a></td>
    <td>Shows communication between the game tasks.</td>
  </tr>
  <tr>
    <td><a href="docs/05-signal-execution-flow.md">05-signal-execution-flow.md</a></td>
    <td>Lists active signals and shows the game state machine.</td>
  </tr>
    <td><a href="docs/06-api-reference.md">06-api-reference.md</a></td>
    <td>Lists the interfaces used when extending the game.</td>
  </tr>
</table>


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

| Type | Bitmap | Notes |
| --- | --- | --- |
| Stone | ![Stone obstacle](/hardware/images/guide/nb_bitmap_stone.png) | Small rock that blocks the lane and must be avoided. |
| Barrier | ![Barrier obstacle](/hardware/images/guide/nb_bitmap_barrier.png) | Road barrier that forces the player to steer around it. |
| Mini car | ![Mini car obstacle](/hardware/images/guide/nb_bitmap_minicar.png) | Small traffic car that acts like a dynamic obstacle in the lane. |


<h3 align="center">III. Game sequence diagram</h3>

```mermaid
%%{init: {'theme': 'base', 'themeVariables': { 
  'primaryColor': '#f0f0f0', 
  'primaryBorderColor': '#444444', 
  'lineColor': '#FF9800', 
  'textColor': '##FF9800', 
  'labelBoxBkgColor': '#e0e0e0', 
  'labelBoxBorderColor': '#444444', 
  'noteBkgColor': '#fafafa', 
  'noteBorderColor': '#666666', 
  'noteTextColor': '#007ACC',
  'activationBkgColor': '#e0e0e0',
  'activationBorderColor': '#444444',
  'signalColor': '#FF9800',
  'signalTextColor': '#007ACC',
  'actorLineColor': '#444444'
}}}%%

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

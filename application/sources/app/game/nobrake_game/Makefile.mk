CFLAGS		+= -I./sources/app/game/nobrake_game
CPPFLAGS	+= -I./sources/app/game/nobrake_game

VPATH += sources/app/game/nobrake_game

# CPP source files
# Nobrake game
SOURCES_CPP += sources/app/game/nobrake_game/nb_game_track.cpp
SOURCES_CPP += sources/app/game/nobrake_game/nb_game_obstacle.cpp
SOURCES_CPP += sources/app/game/nobrake_game/nb_game_car.cpp

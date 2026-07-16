CFLAGS		+= -I./sources/app/screens
CPPFLAGS	+= -I./sources/app/screens

VPATH += sources/app/screens

# CPP source files
SOURCES_CPP += sources/app/screens/scr_idle.cpp
SOURCES_CPP += sources/app/screens/scr_startup.cpp
SOURCES_CPP += sources/app/screens/scr_welcome.cpp
#Screen
SOURCES_CPP += sources/app/screens/scr_menu_game.cpp
SOURCES_CPP += sources/app/screens/scr_nobrake_game.cpp
SOURCES_CPP += sources/app/screens/scr_game_finish.cpp
SOURCES_CPP += sources/app/screens/scr_game_over.cpp
SOURCES_CPP += sources/app/screens/scr_game_score.cpp
SOURCES_CPP += sources/app/screens/scr_game_settings.cpp
SOURCES_CPP += sources/app/screens/screens_bitmap.cpp

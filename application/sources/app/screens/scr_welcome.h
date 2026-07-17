#ifndef __SCR_WELCOME_H__
#define __SCR_WELCOME_H__

#include "fsm.h"
#include "app.h"
#include "task_display.h"
#include "view_render.h"

extern view_dynamic_t dyn_view_welcome;
extern view_screen_t scr_welcome;
extern void scr_welcome_handle(ak_msg_t* msg);

#endif

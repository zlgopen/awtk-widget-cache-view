/**
 * File:   main.c
 * Author: AWTK Develop Team
 * Brief:  main window sources
 *
 * Copyright (c) 2020 - 2020  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */

/**
 * History:
 * ================================================================
 * 2020-05-29 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "awtk.h"
#include "cache_view/cache_view.h"

static ret_t on_close(void* ctx, event_t* e) {
  tk_quit();

  return RET_OK;
}

static ret_t on_disable(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* check_button = WIDGET(e->target);
  widget_t* cache_view = widget_lookup_by_type(win, WIDGET_TYPE_CACHE_VIEW, TRUE);
  bool_t enable = !widget_get_value(check_button);

  widget_set_enable(cache_view, enable);

  return RET_OK;
}

ret_t application_init() {
  widget_t* win = NULL;

  cache_view_register();

  win = window_open("main");
  window_manager_set_show_fps(window_manager(), TRUE);
  widget_child_on(win, "close", EVT_CLICK, on_close, win);
  widget_child_on(win, "disable", EVT_CLICK, on_disable, win);

  return RET_OK;
}

ret_t application_exit() {
  return RET_OK;
}

#include "awtk_main.inc"

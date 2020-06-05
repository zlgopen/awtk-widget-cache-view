/**
 * File:   cache_view.c
 * Author: AWTK Develop Team
 * Brief:  cache view
 *
 * Copyright (c) 2020 - 2020 Guangzhou ZHIYUAN Electronics Co.,Ltd.
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
 * 2020-06-04 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "tkc/mem.h"
#include "tkc/utils.h"
#include "tkc/time_now.h"
#include "base/widget_vtable.h"

#include "cache_view.h"
#include "lcd/lcd_mem_bgr565.h"
#include "lcd/lcd_mem_rgb565.h"
#include "lcd/lcd_mem_rgba8888.h"

static ret_t cache_view_reset_canvas(widget_t* widget);

ret_t cache_view_set_update_interval(widget_t* widget, uint32_t update_interval) {
  cache_view_t* cache_view = CACHE_VIEW(widget);
  return_value_if_fail(cache_view != NULL, RET_BAD_PARAMS);

  cache_view->update_interval = update_interval;

  return RET_OK;
}

static ret_t cache_view_get_prop(widget_t* widget, const char* name, value_t* v) {
  cache_view_t* cache_view = CACHE_VIEW(widget);
  return_value_if_fail(cache_view != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(CACHE_VIEW_PROP_UPDATE_INTERVAL, name)) {
    value_set_uint32(v, cache_view->update_interval);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t cache_view_set_prop(widget_t* widget, const char* name, const value_t* v) {
  return_value_if_fail(widget != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(CACHE_VIEW_PROP_UPDATE_INTERVAL, name)) {
    cache_view_set_update_interval(widget, value_uint32(v));
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t cache_view_on_destroy(widget_t* widget) {
  cache_view_t* cache_view = CACHE_VIEW(widget);
  return_value_if_fail(widget != NULL && cache_view != NULL, RET_BAD_PARAMS);

  cache_view_reset_canvas(widget);

  return RET_OK;
}

static ret_t cache_view_reset_canvas(widget_t* widget) {
  cache_view_t* cache_view = CACHE_VIEW(widget);

  if (cache_view->bitmap != NULL) {
    bitmap_destroy(cache_view->bitmap);
    cache_view->bitmap = NULL;
  }

  return RET_OK;
}

static ret_t cache_view_paint(widget_t* widget, canvas_t* c) {
  if (style_is_valid(widget->astyle)) {
    widget_draw_background(widget, c);
  }
  widget_on_paint_children_default(widget, c);

  return RET_OK;
}

static ret_t cache_view_update(widget_t* widget, canvas_t* c) {
  canvas_t canvas;
  lcd_t* lcd = NULL;
  uint8_t* buff = NULL;
  bitmap_t* bitmap = NULL;
  uint32_t cost = 0;
  uint64_t start = time_now_ms();
  cache_view_t* cache_view = CACHE_VIEW(widget);

  return_value_if_fail(cache_view->bitmap != NULL, RET_BAD_PARAMS);

  if (cache_view->last_update != 0 &&
      (cache_view->last_update + cache_view->update_interval) > start) {
    return RET_OK;
  }

  bitmap = cache_view->bitmap;
  buff = bitmap_lock_buffer_for_write(bitmap);
  return_value_if_fail(buff != NULL, RET_BAD_PARAMS);

  if (bitmap->format == BITMAP_FMT_RGBA8888) {
    lcd = lcd_mem_rgba8888_create_single_fb(widget->w, widget->h, buff);
  } else if (bitmap->format == BITMAP_FMT_BGR565) {
    lcd = lcd_mem_bgr565_create_single_fb(widget->w, widget->h, buff);
  } else {
    lcd = lcd_mem_rgb565_create_single_fb(widget->w, widget->h, buff);
  }

  if (lcd != NULL) {
    canvas_init(&canvas, lcd, c->font_manager);
    canvas_translate(&canvas, c->ox, c->oy);

    canvas_begin_frame(&canvas, NULL, LCD_DRAW_OFFLINE);
    cache_view_paint(widget, &canvas);
    canvas_end_frame(&canvas);

    bitmap->flags |= BITMAP_FLAG_CHANGED;

    canvas_reset(&canvas);
    lcd_destroy(lcd);
  }
  bitmap_unlock_buffer(bitmap);

  cost = time_now_ms() - start;
  log_debug("update cost=%u\n", cost);
  cache_view->last_update = time_now_ms();

  return RET_OK;
}

static ret_t cache_view_ensure_canvas(widget_t* widget, canvas_t* c) {
  cache_view_t* cache_view = CACHE_VIEW(widget);

  if (cache_view->bitmap == NULL) {
    uint32_t w = widget->w;
    uint32_t h = widget->h;
    bitmap_format_t format = lcd_get_desired_bitmap_format(c->lcd);
    if (format != BITMAP_FMT_RGBA8888 && format != BITMAP_FMT_RGB565 &&
        format != BITMAP_FMT_BGR565) {
      format = BITMAP_FMT_BGR565;
    }

    cache_view->bitmap = bitmap_create_ex(w, h, 0, format);
  }

  return RET_OK;
}

static ret_t cache_view_on_paint_children(widget_t* widget, canvas_t* c) {
  cache_view_t* cache_view = CACHE_VIEW(widget);

  if (c->lcd->ratio == 1 && !(cache_view->disable_cache)) {
    uint32_t cost = 0;
    uint64_t start = time_now_ms();
    return_value_if_fail(cache_view_ensure_canvas(widget, c) == RET_OK, RET_FAIL);
    return_value_if_fail(cache_view_update(widget, c) == RET_OK, RET_FAIL);

    canvas_draw_image_at(c, cache_view->bitmap, 0, 0);
    cost = time_now_ms() - start;

    log_debug("cache view paint cost:%u\n", cost);
  } else {
    cache_view_paint(widget, c);
  }

  return RET_OK;
}

static ret_t cache_view_on_event(widget_t* widget, event_t* e) {
  cache_view_t* cache_view = CACHE_VIEW(widget);
  return_value_if_fail(widget != NULL && cache_view != NULL, RET_BAD_PARAMS);

  (void)cache_view;

  return RET_OK;
}

const char* s_cache_view_properties[] = {CACHE_VIEW_PROP_UPDATE_INTERVAL, NULL};

TK_DECL_VTABLE(cache_view) = {.size = sizeof(cache_view_t),
                              .type = WIDGET_TYPE_CACHE_VIEW,
                              .clone_properties = s_cache_view_properties,
                              .persistent_properties = s_cache_view_properties,
                              .parent = TK_PARENT_VTABLE(widget),
                              .create = cache_view_create,
                              .on_paint_background = NULL,
                              .on_paint_border = NULL,
                              .on_paint_self = NULL,
                              .on_paint_begin = NULL,
                              .on_paint_end = NULL,
                              .on_paint_children = cache_view_on_paint_children,
                              .set_prop = cache_view_set_prop,
                              .get_prop = cache_view_get_prop,
                              .on_event = cache_view_on_event,
                              .on_destroy = cache_view_on_destroy};

widget_t* cache_view_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t* widget = widget_create(parent, TK_REF_VTABLE(cache_view), x, y, w, h);
  cache_view_t* cache_view = CACHE_VIEW(widget);
  return_value_if_fail(cache_view != NULL, NULL);

  cache_view->update_interval = 200;

  return widget;
}

widget_t* cache_view_cast(widget_t* widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, cache_view), NULL);

  return widget;
}

#include "base/widget_factory.h"

ret_t cache_view_register(void) {
  return widget_factory_register(widget_factory(), WIDGET_TYPE_CACHE_VIEW, cache_view_create);
}

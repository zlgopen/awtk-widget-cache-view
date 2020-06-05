#include "cache_view/cache_view.h"
#include "gtest/gtest.h"

TEST(cache_view, basic) {
  value_t v;
  widget_t* w = cache_view_create(NULL, 10, 20, 30, 40);

  widget_destroy(w);
}

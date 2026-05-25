#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_OBJECTS 1024

#define WIN_WIDTH 60
#define WIN_HEIGHTS 40

#define ANSI_CLEAR_WIN "\033[%dA\033[%dD"

static char win_buffer[WIN_WIDTH * WIN_HEIGHTS + 1] = {' '};

#define WIN_AT(x, y) win_buffer + ((y) * WIN_WIDTH + (x))

typedef struct Object Object;
typedef void(ObjectUpdateFn)(Object *this);

struct Object {
  uint16_t x;
  uint16_t y;
  float update_ms;
  ObjectUpdateFn *update;
};

static Object *objects[MAX_OBJECTS] = {0};
static size_t objects_count = 0;

typedef struct ScrollingText {
  Object header;
  const char *text;
  uint16_t width;
  uint16_t offset;
} ScrollingText;

void update_scrolling_text(Object *this) {
  ScrollingText *obj = (ScrollingText *)this;
  strncpy(WIN_AT(obj->header.x, obj->header.y), obj->text + obj->offset, obj->width);
  obj->offset = (obj->offset + 1) % strlen(obj->text);
}

static ScrollingText scrolling_text = {
    .header =
        {
            .x = 10,
            .y = 10,
            .update_ms = 200,
            .update = update_scrolling_text,
        },
    .text = "THIS TEXT TOO LONG SO IT SCROLLS EVERY 0.2 SECONDS PER CHAR   ",
    .width = 15,
    .offset = 0,
};

void append_object(Object *obj) { objects[objects_count++] = obj; }

bool update_field(__useconds_t slept) {
  bool is_field_changed = false;

  for (size_t i = 0; i < objects_count; i++) {
    Object *obj = objects[i];
    // 20 0000
    __useconds_t update_ns = obj->update_ms * 1000;
    /* printf("%d\n", slept % update_ns); */
    if (slept % update_ns * 2 >= update_ns) {
      obj->update(obj);
      is_field_changed = true;
    }
  }

  return is_field_changed;
}

void draw_field(void) {
  printf(ANSI_CLEAR_WIN, WIN_HEIGHTS, WIN_WIDTH);
  for (size_t i = 0; i < WIN_HEIGHTS * WIN_WIDTH; i++) {
    printf("%c", win_buffer[i]);

    if (i % WIN_WIDTH == WIN_WIDTH - 1) {
      printf("\n");
    }
  }

  fflush(stdout);
}

int main(void) {
  win_buffer[WIN_HEIGHTS * WIN_WIDTH - 1] = '\0';
  append_object(&scrolling_text.header);
  bool run = true;
  __useconds_t slept = 0;

  while (run) {
    if (update_field(slept)) {
      draw_field();
    }

    usleep(100);
    slept += 100;
  }

  return 0;
}

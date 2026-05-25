#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>

#define LOGCIE_IMPLEMENTATION
#include "./deps/logcie.h"

static const char *logcie_module = "TETRIS";

#define MAX_OBJECTS 1024

#define WIN_WIDTH 60
#define WIN_HEIGHTS 40

#define ANSI_CLEAR_WIN "\033[2J"
#define ANSI_HOME "\033[H"
#define ANSI_CURSOR_HIDE "\033[?25l"
#define ANSI_CURSOR_SHOW "\033[?25h"

static FILE *log_file = NULL;

void setup_logcie(void) {
  log_file = fopen("tetris.log", "w");

  if (!log_file) {
    LOGCIE_ERROR("Cold not open log file: %s", strerror(errno));
    return;
  }

  static Logcie_Sink sink = {
    .formatter = { logcie_printf_formatter, "[$L] $m" },
    .writer = { logcie_printf_writer, NULL },
    .filter = logcie_filter_level_min(LOGCIE_LEVEL_DEBUG),
  };

  sink.writer.data = log_file;

  logcie_add_sink(&sink);
  LOGCIE_DEBUG("Logcie initialized successfuly!");
}

static char win_buffer[WIN_WIDTH * WIN_HEIGHTS + 1] = {' '};

#define WIN_AT(x, y) win_buffer + ((y) * WIN_WIDTH + (x))

typedef struct Object Object;
typedef void(ObjectUpdateFn)(Object *obj);

struct Object {
  uint16_t x;
  uint16_t y;
  float update_ms;
  uint64_t last_update;
  ObjectUpdateFn *update;
};

static Object *objects[MAX_OBJECTS] = {0};
static size_t objects_count = 0;

// ==============================

typedef struct ManHello {
  Object header;
  int state;
} ManHello;

void update_man_hello(Object *obj) {
  ManHello *this = (ManHello *)obj;

  static const char *man[4][3] = {
    {
      " 0|",
      "/|",
      "/\\",
    },
    {
      " 0/",
      "/|",
      "/\\",
    }, {
      " 0",
      "/|\\",
      "/\\",
    }, {
      " 0/",
      "/|",
      "/\\",
    }
  };

  for (uint8_t i = 0; i < 3; i++) {
    memset(WIN_AT(this->header.x, this->header.y + i), ' ', 5);
    strncpy(WIN_AT(this->header.x, this->header.y + i), man[this->state][i], 5);
  }

  this->state = (this->state + 1) % 4;
}

static ManHello man_hello = {
  .header = {
    .x = 30,
    .y = 2,
    .update_ms = 500,
    .last_update = 0,
    .update = update_man_hello,
  },
  .state = 0,
};

// ==============================

typedef struct ScrollingText {
  Object header;
  const char *text;
  uint16_t width;
  uint16_t offset;
} ScrollingText;

void update_scrolling_text(Object *obj) {
  ScrollingText *this = (ScrollingText *)obj;

  size_t text_len = strlen(this->text);
  size_t copy_len = this->width;

  if (this->offset + copy_len > text_len) {
    copy_len = text_len - this->offset;
    strncpy(WIN_AT(this->header.x + copy_len, this->header.y), this->text, this->width - copy_len);
  }

  strncpy(WIN_AT(this->header.x, this->header.y), this->text + this->offset, copy_len);
  this->offset = (this->offset + 1) % text_len;
}

static ScrollingText scrolling_text = {
    .header =
        {
            .x = 10,
            .y = 10,
            .update_ms = 200,
            .last_update = 0,
            .update = update_scrolling_text,
        },
    .text = "THIS TEXT TOO LONG SO IT SCROLLS EVERY 0.2 SECONDS PER CHAR   ",
    .width = 15,
    .offset = 0,
};

// ==============================

typedef struct Counter {
  Object header;
  uint64_t coutner;
  const char *label;
} Counter;

void counter_update(Object *obj) {
  Counter *this = (Counter *)obj;

  size_t len = strlen(this->label) + 10; // 10 is max length for max UINT32 number
  snprintf(WIN_AT(obj->x, obj->y), len, "%s%zu", this->label, this->coutner);
  this->coutner++;
}

static Counter counter = {
  .header = {
    .x = 15,
    .y = 16,
    .update_ms = 1000,
    .last_update = 0,
    .update = counter_update,
  },
  .coutner = 0,
  .label = "1 second counter: ",
};

// ==============================

void append_object(Object *obj) { objects[objects_count++] = obj; }

uint64_t get_time_ms(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (uint64_t)(tv.tv_sec) * 1000 + (tv.tv_usec / 1000);
}

bool update_field(uint64_t current_time) {
  bool is_field_changed = false;

  for (size_t i = 0; i < objects_count; i++) {
    Object *obj = objects[i];

    if (current_time - obj->last_update >= obj->update_ms) {
      obj->update(obj);
      obj->last_update = current_time;
      is_field_changed = true;
    }
  }

  return is_field_changed;
}

void draw_field(void) {
  printf(ANSI_HOME);

  for (size_t i = 0; i < WIN_HEIGHTS * WIN_WIDTH; i++) {
    putchar(win_buffer[i]);

    if ((i + 1) % WIN_WIDTH == 0) {
      putchar('\n');
    }
  }

  fflush(stdout);
}

int main(void) {
  setup_logcie();

  memset(win_buffer, ' ', WIN_HEIGHTS * WIN_WIDTH);
  win_buffer[WIN_HEIGHTS * WIN_WIDTH - 1] = '\0';

  append_object(&scrolling_text.header);
  append_object(&counter.header);
  append_object(&man_hello.header);

  printf(ANSI_CLEAR_WIN ANSI_CURSOR_HIDE);

  bool run = true;
  const int frame_delay_ms = 16; // ~60 FPS

  while (run) {
    uint64_t current_time = get_time_ms();

    if (update_field(current_time)) {
      draw_field();
    }

    usleep(frame_delay_ms * 1000);
  }

  printf(ANSI_CURSOR_SHOW);

  if (log_file) {
    fclose(log_file);
  }

  return 0;
}

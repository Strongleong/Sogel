#include <errno.h>  // IWYU pragma: keep
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>

#define SOGEL_IMPLEMENTATION
#include "sogel.h"

#define LOGCIE_IMPLEMENTATION
#include "./deps/logcie.h"

static FILE *log_file = NULL;

void setup_logcie(void) {
  log_file = fopen("tetris.log", "w");

  if (!log_file) {
    LOGCIE_ERROR("Cold not open log file: %s", strerror(errno));
    return;
  }

  static Logcie_Sink sink = {
    .formatter = {logcie_printf_formatter, "[$L] $m"},
    .writer    = {logcie_printf_writer, NULL},
    .filter    = logcie_filter_level_min(LOGCIE_LEVEL_DEBUG),
  };

  sink.writer.data = log_file;

  logcie_add_sink(&sink);
  LOGCIE_DEBUG("Logcie initialized successfuly!");
}

typedef struct ManHello {
  Object header;
  int    state;
} ManHello;

void update_man_hello(Object *obj) {
  ManHello *this = (ManHello *)obj;

  static const char *man[4][3] = {
    {
      " 0|",
      "/|",
      "/ \\",
    },
    {
      " 0/",
      "/|",
      "/ \\",
    },
    {
      " 0",
      "/|\\",
      "/ \\",
    },
    {
      " 0/",
      "/|",
      "/ \\",
    }
  };

  for (uint8_t i = 0; i < 3; i++) {
    memset(sogel_at(this->header.x, this->header.y + i), ' ', 5);
    strncpy(sogel_at(this->header.x, this->header.y + i), man[this->state][i], 5);
  }

  this->state = (this->state + 1) % 4;
}

static ManHello man_hello = {
  .header = {
    .x           = 30,
    .y           = 2,
    .update_ms   = 500,
    .last_update = 0,
    .update      = update_man_hello,
  },
  .state = 0,
};

// ==============================

typedef struct ScrollingText {
  Object      header;
  const char *text;
  uint16_t    width;
  uint16_t    offset;
} ScrollingText;

void update_scrolling_text(Object *obj) {
  ScrollingText *this = (ScrollingText *)obj;

  size_t text_len = strlen(this->text);
  size_t copy_len = this->width;

  if (this->offset + copy_len > text_len) {
    copy_len = text_len - this->offset;
    strncpy(sogel_at(this->header.x + copy_len, this->header.y), this->text, this->width - copy_len);
  }

  strncpy(sogel_at(this->header.x, this->header.y), this->text + this->offset, copy_len);
  this->offset = (this->offset + 1) % text_len;
}

static ScrollingText scrolling_text = {
  .header = {
    .x           = 10,
    .y           = 10,
    .update_ms   = 200,
    .last_update = 0,
    .update      = update_scrolling_text,
  },
  .text   = "THIS TEXT TOO LONG SO IT SCROLLS EVERY 0.2 SECONDS PER CHAR   ",
  .width  = 15,
  .offset = 0,
};

// ==============================

typedef struct Counter {
  Object      header;
  uint64_t    coutner;
  const char *label;
} Counter;

void counter_update(Object *obj) {
  Counter *this = (Counter *)obj;

  size_t len = strlen(this->label) + 10;  // 10 is max length for max UINT32 number
  snprintf(sogel_at(obj->x, obj->y), len, "%s%zu", this->label, this->coutner);
  this->coutner++;
}

static Counter counter = {
  .header = {
    .x           = 15,
    .y           = 16,
    .update_ms   = 1000,
    .last_update = 0,
    .update      = counter_update,
  },
  .coutner = 0,
  .label   = "1 second counter: ",
};

// ==============================

static bool run = true;

void intHandler(int dummy) {
  (void)dummy;
  run = false;
}

int main(void) {
  signal(SIGINT, intHandler);

  setup_logcie();

  sogel_set_size(60, 40);
  sogel_set_fps(60);
  sogel_hide_cursor();
  printf(SOGEL_ANSI_CLEAR_SCREEN);

  sogel_add_object(&scrolling_text.header);
  sogel_add_object(&counter.header);
  sogel_add_object(&man_hello.header);

  while (run) {
    uint64_t now = sogel_get_time_ms();

    if (sogel_update(now)) {
      sogel_render();
    }

    sogel_sleep_us(sogel_get_frame_delay_us());
  }

  sogel_show_cursor();

  if (log_file) {
    fclose(log_file);
  }

  return 0;
}

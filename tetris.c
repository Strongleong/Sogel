#include <errno.h>  // IWYU pragma: keep
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#define LOGCIE_IMPLEMENTATION
#include "./deps/logcie.h"

#define SOGEL_IMPLEMENTATION
#include "sogel.h"

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
    .filter    = logcie_filter_level_max(LOGCIE_LEVEL_VERBOSE),
  };

  sink.writer.data = log_file;

  logcie_add_sink(&sink);
  LOGCIE_DEBUG("Logcie initialized successfuly!");
}

// ==============================

typedef struct ManHello {
  Object     header;
  int        state;
  SogelTimer anim_timer;
} ManHello;

void update_man_hello(Object *obj) {
  ManHello *this = (ManHello *)obj;

  if (sogel_is_key_down(SOGEL_KEY_LEFT) && this->header.x > 0) {
    this->header.x--;
  }

  if (sogel_is_key_down(SOGEL_KEY_RIGHT) && this->header.x < sogel_get_width() - 5) {
    this->header.x++;
  }

  if (sogel_is_key_down(SOGEL_KEY_UP) && this->header.y > 0) {
    this->header.y--;
  }

  if (sogel_is_key_down(SOGEL_KEY_DOWN) && this->header.y < sogel_get_height() - 5) {
    this->header.y++;
  }
}

void draw_man_hello(Object *obj) {
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

  if (sogel_timer_elapsed(&this->anim_timer)) {
    this->state = (this->state + 1) % 4;
  }

  for (uint8_t i = 0; i < 3; i++) {
    memset(sogel_at(this->header.x, this->header.y + i), ' ', 5);
    strncpy(sogel_at(this->header.x, this->header.y + i), man[this->state][i], 5);
  }
}

static ManHello man_hello = {
  .header = {
    .x      = 30,
    .y      = 2,
    .draw   = draw_man_hello,
    .update = update_man_hello,
  },
  .state      = 0,
  .anim_timer = {500, 0},
};

// ==============================

typedef struct ScrollingText {
  Object      header;
  const char *text;
  uint16_t    width;
  uint16_t    offset;
  SogelTimer  scroll_timer;
} ScrollingText;

void draw_scrolling_text(Object *obj) {
  ScrollingText *this = (ScrollingText *)obj;

  if (sogel_timer_elapsed(&this->scroll_timer)) {
    this->offset = (this->offset + 1) % strlen(this->text);
  }

  size_t text_len = strlen(this->text);
  size_t copy_len = this->width;

  if (this->offset + copy_len > text_len) {
    copy_len = text_len - this->offset;
    strncpy(sogel_at(this->header.x + copy_len, this->header.y), this->text, this->width - copy_len);
  }

  strncpy(sogel_at(this->header.x, this->header.y), this->text + this->offset, copy_len);
}

static ScrollingText scrolling_text = {
  .header = {
    .x      = 10,
    .y      = 10,
    .draw   = draw_scrolling_text,
    .update = NULL,
  },
  .text         = "THIS TEXT TOO LONG SO IT SCROLLS EVERY 0.2 SECONDS PER CHAR   ",
  .width        = 15,
  .offset       = 0,
  .scroll_timer = {200, 0},
};

// ==============================

typedef struct Counter {
  Object      header;
  uint64_t    counter;
  const char *label;
  SogelTimer  inc_timer;
} Counter;

void draw_counter(Object *obj) {
  Counter *this = (Counter *)obj;

  if (sogel_timer_elapsed(&this->inc_timer)) {
    this->counter++;
  }

  size_t len = strlen(this->label) + 10;  // 10 is max length for max UINT32 number
  snprintf(sogel_at(obj->x, obj->y), len, "%s%zu", this->label, this->counter);
}

static Counter counter = {
  .header = {
    .x      = 15,
    .y      = 16,
    .draw   = draw_counter,
    .update = NULL,
  },
  .counter   = 0,
  .label     = "1 second counter: ",
  .inc_timer = {1000, 0},
};

// ==============================

void draw_help_text(Object *obj) {
  const char *text = "Press Esc to quit. Use <UP>, <DOWN>, <LEFT> and <RIGHT> keys to control the waving man";
  snprintf(sogel_at(obj->x, obj->y), strlen(text) + 2, "%s", text);
}

static Object help_text = {
  .x      = 0,
  .y      = 0,
  .draw   = draw_help_text,
  .update = NULL,
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
  sogel_clear_term();
  sogel_setup_input();

  sogel_add_object(&help_text);
  sogel_add_object(&scrolling_text.header);
  sogel_add_object(&counter.header);
  sogel_add_object(&man_hello.header);

  while (run) {
    sogel_poll_events();

    if (sogel_is_key_pressed(SOGEL_KEY_ESC)) {
      run = false;
    }

    uint64_t now = sogel_get_time_ms();

    sogel_clear();
    sogel_tick(now);
    sogel_render();

    sogel_sleep_us(sogel_get_frame_delay_us());
  }

  sogel_show_cursor();

  if (log_file) {
    fclose(log_file);
  }

  return 0;
}

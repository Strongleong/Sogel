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
  log_file = fopen("game.log", "w");

  if (!log_file) {
    LOGCIE_ERROR("Cold not open log file: %s", strerror(errno));
    return;
  }

  static Logcie_Sink sink = {
    .formatter = {logcie_printf_formatter, "[$L] $m"},
    .writer    = {logcie_printf_writer, NULL},
    .filter    = logcie_filter_level_min(LOGCIE_LEVEL_VERBOSE),
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

void update_man_hello(Object *obj, uint64_t delta_ms) {
  ManHello *this = (ManHello *)obj;
  (void)delta_ms;

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

void draw_man_hello(Object *obj, uint64_t delta_ms) {
  ManHello *this = (ManHello *)obj;
  (void)delta_ms;

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

void draw_scrolling_text(Object *obj, uint64_t delta_ms) {
  ScrollingText *this = (ScrollingText *)obj;
  (void)delta_ms;

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

void draw_counter(Object *obj, uint64_t delta_ms) {
  Counter *this = (Counter *)obj;
  (void)delta_ms;

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

typedef struct {
  Object header;
  float  x_sub;
  float  y_sub;
  float  speed_x;
  float  speed_y;
  bool   use_delta;
} Ball;

void update_ball(Object *obj, uint64_t delta_ms) {
  Ball *this = (Ball *)obj;

  float delta_sec = delta_ms / 1000.0f;

  if (this->use_delta) {
    this->x_sub += this->speed_x * delta_sec;
    this->y_sub += this->speed_y * delta_sec;
  } else {
    this->x_sub += this->speed_x;
    this->y_sub += this->speed_y;
  }

  if (this->x_sub < 0) {
    this->x_sub   = 0;
    this->speed_x = -this->speed_x;
  } else if (this->x_sub > sogel_get_width() - 1) {
    this->x_sub   = sogel_get_width() - 1.0f;
    this->speed_x = -this->speed_x;
  }

  if (this->y_sub < 0) {
    this->y_sub   = 0;
    this->speed_y = -this->speed_y;
  } else if (this->y_sub > sogel_get_height() - 1) {
    this->y_sub   = sogel_get_height() - 1.0f;
    this->speed_y = -this->speed_y;
  }

  this->header.x = (uint16_t)this->x_sub;
  this->header.y = (uint16_t)this->y_sub;

  if (sogel_is_key_down(SOGEL_KEY_SPACE)) {
    this->use_delta = !this->use_delta;
  }
}

void draw_ball(Object *obj, uint64_t delta_ms) {
  Ball *this = (Ball *)obj;
  (void)delta_ms;

  *sogel_at(this->header.x, this->header.y) = 'O';
}

static Ball ball = {
  .header = {
    .x      = 10,
    .y      = 10,
    .update = update_ball,
    .draw   = draw_ball,
  },
  .x_sub     = 10.0f,
  .y_sub     = 10.0f,
  .speed_x   = 15.0f,
  .speed_y   = 8.0f,
  .use_delta = true,
};

// ==============================

void draw_help_text(Object *obj, uint64_t delta_ms) {
  (void)delta_ms;
  const char *text =
    "Press Esc to quit. Use <UP>, <DOWN>, <LEFT> and <RIGHT> keys to control the waving man."
    "Ball uses delts: ";
  snprintf(sogel_at(obj->x, obj->y), strlen(text) + 6, "%s%s", text, ball.use_delta ? "yes" : "no");
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
  sogel_add_object(&ball.header);
  sogel_add_object(&scrolling_text.header);
  sogel_add_object(&counter.header);
  sogel_add_object(&man_hello.header);

  while (run) {
    sogel_poll_events();

    if (sogel_is_key_pressed(SOGEL_KEY_ESC)) {
      run = false;
    }

    sogel_clear();
    sogel_tick();
    sogel_render();

    sogel_sleep_us(sogel_get_frame_delay_us());
  }

  sogel_show_cursor();

  if (log_file) {
    fclose(log_file);
  }

  return 0;
}

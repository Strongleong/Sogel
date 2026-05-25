#ifndef SOGEL_H
#define SOGEL_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#define SOGEL_VERSION_MAJOR         0
#define SOGEL_VERSION_MINOR         5
#define SOGEL_VERSION_RELEASE       0
#define SOGEL_VERSION_NUMBER        (SOGEL_VERSION_MAJOR * 100 * 100 + SOGEL_VERSION_MINOR * 100 + SOGEL_VERSION_RELEASE)
#define SOGEL_VERSION_FULL          SOGEL_VERSION_MAJOR.SOGEL_VERSION_MINOR.SOGEL_VERSION_RELEASE
#define SOGEL_QUOTE(str)            #str
#define SOGEL_EXPAND_AND_QUOTE(str) SOGEL_QUOTE(str)
#define SOGEL_VERSION_STRING        SOGEL_EXPAND_AND_QUOTE(SOGEL_VERSION_FULL)

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
#define SOGEL_PLATFORM_UNIX
#include <sys/time.h>
#include <unistd.h>
#elif defined(_WIN32)
#error "Windows is not supported for now"
// #define SOGEL_PLATFORM_WINDOWS
// #include <windows.h>
#else
#error "Unsupported platform"
#endif

#ifndef SOGEL_MAX_WIDTH
#define SOGEL_MAX_WIDTH 200
#endif

#ifndef SOGEL_MAX_HEIGHT
#define SOGEL_MAX_HEIGHT 100
#endif

#ifndef SOGEL_MAX_OBJECTS
#define SOGEL_MAX_OBJECTS 1024
#endif

#define SOGEL_BACKEND_TERMINAL 0

#ifndef SOGEL_BACKEND
#define SOGEL_BACKEND SOGEL_BACKEND_TERMINAL
#endif

#if SOGEL_BACKEND == SOGEL_BACKEND_TERMINAL
#define SOGEL_ANSI_CLEAR_SCREEN "\033[2J"
#define SOGEL_ANSI_HOME         "\033[H"
#define SOGEL_ANSI_CURSOR_HIDE  "\033[?25l"
#define SOGEL_ANSI_CURSOR_SHOW  "\033[?25h"
#else
#error "Only Unix+Term is supported"
#endif

#if SOGEL_BACKEND == SOGEL_BACKEND_TERMINAL
#define SOGEL_KEY_UP    256
#define SOGEL_KEY_DOWN  257
#define SOGEL_KEY_LEFT  258
#define SOGEL_KEY_RIGHT 259
#define SOGEL_KEY_ESC   27
#define SOGEL_KEY_SPACE ' '
#define SOGEL_KEY_ENTER '\n'
// TODO: Fill up this
#else
#error "Keycodes only Unix+Term are supported"
#endif

#ifndef SOGEL_LOG
#ifdef LOGCIE
#ifdef LOGCIE_VA_LOGS
#define SOGEL_LOG(level, ...) LOGCIE_##level##_VA(__VA_ARGS__)
#else
#define SOGEL_LOG(level, ...) LOGCIE_##level(__VA_ARGS__)
#endif
#else
#define SOGEL_LOG(level, ...) ((void)0)
#endif
#endif

#define SOGEL_LOG_TRACE(...)   SOGEL_LOG(TRACE, __VA_ARGS__)
#define SOGEL_LOG_DEBUG(...)   SOGEL_LOG(DEBUG, __VA_ARGS__)
#define SOGEL_LOG_VERBOSE(...) SOGEL_LOG(VERBOSE, __VA_ARGS__)
#define SOGEL_LOG_INFO(...)    SOGEL_LOG(INFO, __VA_ARGS__)
#define SOGEL_LOG_WARN(...)    SOGEL_LOG(WARN, __VA_ARGS__)
#define SOGEL_LOG_ERROR(...)   SOGEL_LOG(ERROR, __VA_ARGS__)
#define SOGEL_LOG_FATAL(...)   SOGEL_LOG(FATAL, __VA_ARGS__)

#ifndef SOGEL_ASSERT
#define SOGEL_ASSERT(cond, msg)                     \
  do {                                              \
    if (!(cond)) {                                  \
      SOGEL_LOG_FATAL("Assertion failed: %s", msg); \
      assert((cond) && (msg));                      \
    }                                               \
  } while (0)
#endif

#ifndef SOGEL_DEF
#define SOGEL_DEF extern
#endif

typedef struct Object Object;
typedef void(ObjectUpdateFn)(Object *obj);
typedef void(ObjectDrawFn)(Object *obj);

struct Object {
  uint16_t        x;
  uint16_t        y;
  ObjectUpdateFn *update;
  ObjectUpdateFn *draw;
};

typedef struct SogelTimer {
  uint32_t interval_ms;
  uint64_t last_time;
} SogelTimer;

// Configuration
SOGEL_DEF void     sogel_set_fps(uint32_t fps);
SOGEL_DEF uint32_t sogel_get_fps(void);
SOGEL_DEF uint64_t sogel_get_frame_delay_us(void);
SOGEL_DEF void     sogel_set_size(uint16_t width, uint16_t height);
SOGEL_DEF uint16_t sogel_get_width(void);
SOGEL_DEF uint16_t sogel_get_height(void);
SOGEL_DEF void     sogel_hide_cursor(void);
SOGEL_DEF void     sogel_show_cursor(void);
// Utility
SOGEL_DEF void     sogel_clear(void);
SOGEL_DEF void     sogel_clear_term(void);
SOGEL_DEF char    *sogel_at(uint16_t x, uint16_t y);
// Object management
SOGEL_DEF void     sogel_add_object(Object *obj);
// Game loop
SOGEL_DEF void     sogel_tick(uint64_t now_ms);
SOGEL_DEF uint64_t sogel_get_time_ms(void);
SOGEL_DEF void     sogel_sleep_us(uint64_t microseconds);
SOGEL_DEF void     sogel_render(void);
SOGEL_DEF bool     sogel_timer_elapsed(SogelTimer *t);
// Event system
SOGEL_DEF void     sogel_setup_input(void);
SOGEL_DEF void     sogel_poll_events(void);
SOGEL_DEF bool     sogel_is_key_down(uint16_t key);
SOGEL_DEF bool     sogel_is_key_pressed(uint16_t key);
SOGEL_DEF bool     sogel_is_key_released(uint16_t key);
SOGEL_DEF uint16_t sogel_get_char(void);

#ifdef SOGEL_IMPLEMENTATION

#include <assert.h>
#include <stdio.h>
#include <string.h>

#ifdef LOGCIE
static const char *logcie_module = "sogel";
#endif

static uint16_t sogel_width          = 80;
static uint16_t sogel_height         = 24;
static uint32_t sogel_fps            = 60;
static uint64_t sogel_frame_delay_us = 16667;

static char sogel_buffer[SOGEL_MAX_WIDTH * SOGEL_MAX_HEIGHT + 1];

static Object *sogel_objects[SOGEL_MAX_OBJECTS];
static size_t  sogel_objects_count = 0;

#define SOGEL_MAX_KEY 512

static bool     sogel_keys_curr[SOGEL_MAX_KEY] = {0};
static bool     sogel_keys_prev[SOGEL_MAX_KEY] = {0};
static uint16_t sogel_last_char                = 0;

#ifdef SOGEL_PLATFORM_UNIX
#include <fcntl.h>
#include <stdlib.h>
#include <termios.h>

static struct termios sogel_orig_termios;
static bool           sogel_termios_saved = false;

static void sogel_terminal_restore(void) {
  if (sogel_termios_saved) {
    tcsetattr(STDIN_FILENO, TCSANOW, &sogel_orig_termios);
  }
}

static void sogel_terminal_enable_raw(void) {
  if (!sogel_termios_saved) {
    int32_t rc = tcgetattr(STDIN_FILENO, &sogel_orig_termios);
    SOGEL_ASSERT(rc == 0, "tcgetattr failed – cannot save terminal settings");
    sogel_termios_saved = true;
    atexit(sogel_terminal_restore);
    SOGEL_LOG_DEBUG("Terminal settings are saved");
  }

  struct termios raw = sogel_orig_termios;
  raw.c_lflag &= ~(ICANON | ECHO);
  int32_t rc = tcsetattr(STDIN_FILENO, TCSANOW, &raw);
  SOGEL_ASSERT(rc == 0, "tcgetattr failed – cannot save terminal settings");

  int32_t flags = fcntl(STDIN_FILENO, F_GETFL, 0);
  SOGEL_ASSERT(flags != -1, "fcntl F_GETFL failed");
  fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

  SOGEL_LOG_DEBUG("Raw input mode enabled");
}
#endif

SOGEL_DEF void sogel_set_fps(uint32_t fps) {
  SOGEL_ASSERT(fps > 0, "FPS must be greater than 0");
  sogel_fps            = fps;
  sogel_frame_delay_us = UINT64_C(1000000) / fps;
  SOGEL_LOG_DEBUG("FPS set to %u (frame delay %lu us)", fps, sogel_frame_delay_us);
}

SOGEL_DEF uint32_t sogel_get_fps(void) {
  return sogel_fps;
}

SOGEL_DEF uint64_t sogel_get_frame_delay_us(void) {
  return sogel_frame_delay_us;
}

SOGEL_DEF void sogel_set_size(uint16_t width, uint16_t height) {
  SOGEL_ASSERT(width > 0 && height > 0, "Width and height must be greater than 0");

  if (width > SOGEL_MAX_WIDTH) {
    SOGEL_LOG_WARN("Width %u exceeds max %d, clamping", width, SOGEL_MAX_WIDTH);
    width = SOGEL_MAX_WIDTH;
  }

  if (height > SOGEL_MAX_HEIGHT) {
    SOGEL_LOG_WARN("Height %u exceeds max %d, clamping", height, SOGEL_MAX_HEIGHT);
    height = SOGEL_MAX_HEIGHT;
  }

  sogel_width  = width;
  sogel_height = height;
  sogel_clear();

  SOGEL_LOG_DEBUG("Window size set to %ux%u", width, height);
}

SOGEL_DEF uint16_t sogel_get_width(void) {
  return sogel_width;
}

SOGEL_DEF uint16_t sogel_get_height(void) {
  return sogel_height;
}

SOGEL_DEF void sogel_hide_cursor(void) {
  printf(SOGEL_ANSI_CURSOR_HIDE);
  fflush(stdout);
  SOGEL_LOG_DEBUG("Cursor hidden");
}

SOGEL_DEF void sogel_show_cursor(void) {
  printf(SOGEL_ANSI_CURSOR_SHOW);
  fflush(stdout);
  SOGEL_LOG_DEBUG("Cursor shown");
}

SOGEL_DEF void sogel_clear(void) {
  memset(sogel_buffer, ' ', (size_t)sogel_width * sogel_height);
  sogel_buffer[sogel_width * sogel_height] = '\0';
  SOGEL_LOG_VERBOSE("Buffer cleared (%ux%u)", sogel_width, sogel_height);
}

SOGEL_DEF void sogel_clear_term(void) {
  printf(SOGEL_ANSI_CLEAR_SCREEN);
  fflush(stdout);
  SOGEL_LOG_DEBUG("Terminal was cleared");
}

SOGEL_DEF char *sogel_at(uint16_t x, uint16_t y) {
  SOGEL_ASSERT(x < sogel_width && y < sogel_height, "Coordinates out of bounds");
  return sogel_buffer + (y * sogel_width + x);
}

SOGEL_DEF void sogel_add_object(Object *obj) {
  SOGEL_ASSERT(obj != NULL, "Object must not be NULL");

  if (sogel_objects_count < SOGEL_MAX_OBJECTS) {
    SOGEL_LOG_DEBUG("Object added (total: %zu)", sogel_objects_count);
    sogel_objects[sogel_objects_count++] = obj;
  } else {
    SOGEL_LOG_ERROR("Cannot add object – max objects %d reached", SOGEL_MAX_OBJECTS);
  }
}

SOGEL_DEF void sogel_tick(uint64_t now_ms) {
  (void)now_ms;

  for (size_t i = 0; i < sogel_objects_count; i++) {
    Object *obj = sogel_objects[i];
    SOGEL_ASSERT(obj != NULL, "Object must not be NULL");

    if (obj->update) {
      obj->update(obj);
      SOGEL_LOG_TRACE("Object %zu update() called", i);
    }

    if (obj->draw) {
      obj->draw(obj);
      SOGEL_LOG_TRACE("Object %zu draw() triggered", i);
    }
  }

  SOGEL_LOG_TRACE("Tick: %zu objects", sogel_objects_count);
}

// Platform-specific

SOGEL_DEF uint64_t sogel_get_time_ms(void) {
#ifdef SOGEL_PLATFORM_UNIX
  struct timeval tv;
  gettimeofday(&tv, NULL);
  uint64_t ms = (uint64_t)(tv.tv_sec) * UINT64_C(1000) +
                (uint64_t)(tv.tv_usec) / UINT64_C(1000);
  SOGEL_LOG_TRACE("Current time: %lu ms", ms);
  return ms;
#else
#error "sogel_get_time_ms" is not implemented for your system yet
#endif
}

SOGEL_DEF void sogel_sleep_us(uint64_t microseconds) {
  SOGEL_LOG_TRACE("Sleeping for %lu us", microseconds);
#ifdef SOGEL_PLATFORM_UNIX
  usleep(microseconds);
#else
#error "sogel_sleep_us" is not implemented for your system yet
#endif
}

SOGEL_DEF void sogel_render(void) {
#if SOGEL_BACKEND == SOGEL_BACKEND_TERMINAL
  SOGEL_LOG_DEBUG("Rendering frame (%ux%u)", sogel_width, sogel_height);
  printf(SOGEL_ANSI_HOME);

  uint32_t total = (uint32_t)sogel_width * sogel_height;
  for (uint32_t i = 0; i < total; i++) {
    putchar(sogel_buffer[i]);

    if ((i + 1) % sogel_width == 0) {
      putchar('\n');
    }
  }

  fflush(stdout);
  SOGEL_LOG_TRACE("Frame rendered, total characters: %u", total);
#else
#error "sogel_render" is not implemented for your platform yet
#endif
}

SOGEL_DEF bool sogel_timer_elapsed(SogelTimer *t) {
  uint64_t now_ms = sogel_get_time_ms();

  if (now_ms - t->last_time >= t->interval_ms) {
    t->last_time = now_ms;
    return true;
  }

  return false;
}

SOGEL_DEF void sogel_setup_input(void) {
#ifdef SOGEL_PLATFORM_UNIX
  sogel_terminal_enable_raw();
#else
#error "sogel_setup_input" is now implemented for your platform yet
#endif
}

SOGEL_DEF void sogel_poll_events(void) {
  memcpy(sogel_keys_prev, sogel_keys_curr, sizeof(sogel_keys_prev));
  memset(sogel_keys_curr, 0, sizeof(sogel_keys_curr));
  sogel_last_char = 0;

#ifdef SOGEL_PLATFORM_UNIX
  SOGEL_ASSERT(sogel_termios_saved, "Input not set up – call sogel_setup_input() first");

  uint8_t buf[32];
  int32_t n = read(STDIN_FILENO, buf, sizeof(buf));
  int32_t i = 0;

  while (i < n) {
    uint8_t ch = buf[i];

    // Detect ANSI escape sequences (e.g., arrow keys)
    if (ch == 27 && i + 2 < n && buf[i + 1] == '[') {
      uint8_t code = buf[i + 2];

      switch (code) {
        case 'A': sogel_keys_curr[SOGEL_KEY_UP] = true; break;
        case 'B': sogel_keys_curr[SOGEL_KEY_DOWN] = true; break;
        case 'C': sogel_keys_curr[SOGEL_KEY_RIGHT] = true; break;
        case 'D': sogel_keys_curr[SOGEL_KEY_LEFT] = true; break;
        default:
          SOGEL_LOG_TRACE("Unknown ANSI sequence: ESC [ %c (0x%02x)", code, code);
          break;
      }

      i += 3;
    } else {
      // Regular char
      sogel_keys_curr[ch] = true;
      sogel_last_char     = ch;
      i++;
    }
  }

  SOGEL_LOG_TRACE("Polled input: %d bytes, last char %u", n, sogel_last_char);
#else
#error "sogel_poll_events" is not implemented for your platform yet
#endif
}

SOGEL_DEF bool sogel_is_key_down(uint16_t key) {
  SOGEL_ASSERT(key < SOGEL_MAX_KEY, "Key code out of valid range");
  return (key < SOGEL_MAX_KEY) ? sogel_keys_curr[key] : false;
}

SOGEL_DEF bool sogel_is_key_pressed(uint16_t key) {
  SOGEL_ASSERT(key < SOGEL_MAX_KEY, "Key code out of valid range");
  if (key >= SOGEL_MAX_KEY) return false;
  return sogel_keys_curr[key] && !sogel_keys_prev[key];
}

SOGEL_DEF bool sogel_is_key_released(uint16_t key) {
  SOGEL_ASSERT(key < SOGEL_MAX_KEY, "Key code out of valid range");
  if (key >= SOGEL_MAX_KEY) return false;
  return !sogel_keys_curr[key] && sogel_keys_prev[key];
}

SOGEL_DEF uint16_t sogel_get_char(void) {
  return sogel_last_char;
}

#endif  // SOGEL_IMPLEMENTATION
#endif  // SOGEL_H

/*
The MIT License (MIT)

Copyright (c) 2026 Nikita (strongleong) Chulkov nikita_chul@mail.ru

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

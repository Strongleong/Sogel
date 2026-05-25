#ifndef SOGEL_H
#define SOGEL_H

#include <stdbool.h>
#include <stdint.h>

#define SOGEL_VERSION_MAJOR         0
#define SOGEL_VERSION_MINOR         2
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

#ifdef LOGCIE
#define SOGEL_LOG_TRACE(...)   LOGCIE_TRACE(__VA_ARGS__)
#define SOGEL_LOG_DEBUG(...)   LOGCIE_DEBUG(__VA_ARGS__)
#define SOGEL_LOG_VERBOSE(...) LOGCIE_VERBOSE(__VA_ARGS__)
#define SOGEL_LOG_INFO(...)    LOGCIE_INFO(__VA_ARGS__)
#define SOGEL_LOG_WARN(...)    LOGCIE_WARN(__VA_ARGS__)
#define SOGEL_LOG_ERROR(...)   LOGCIE_ERROR(__VA_ARGS__)
#define SOGEL_LOG_FATAL(...)   LOGCIE_FATAL(__VA_ARGS__)
#else
#define SOGEL_LOG_TRACE(...)   ((void)0)
#define SOGEL_LOG_DEBUG(...)   ((void)0)
#define SOGEL_LOG_VERBOSE(...) ((void)0)
#define SOGEL_LOG_INFO(...)    ((void)0)
#define SOGEL_LOG_WARN(...)    ((void)0)
#define SOGEL_LOG_ERROR(...)   ((void)0)
#define SOGEL_LOG_FATAL(...)   ((void)0)
#endif

typedef struct Object Object;
typedef void(ObjectUpdateFn)(Object *obj);

struct Object {
  uint16_t        x;
  uint16_t        y;
  uint32_t        update_ms;
  uint64_t        last_update;
  ObjectUpdateFn *update;
};

// Configuration
void     sogel_set_fps(uint32_t fps);
uint32_t sogel_get_fps(void);
uint64_t sogel_get_frame_delay_us(void);
void     sogel_set_size(uint16_t width, uint16_t height);
uint16_t sogel_get_width(void);
uint16_t sogel_get_height(void);
void     sogel_hide_cursor(void);
void     sogel_show_cursor(void);
// Utility
void     sogel_clear(void);
char    *sogel_at(uint16_t x, uint16_t y);
// Object management
void     sogel_add_object(Object *obj);
// Game loop
bool     sogel_update(uint64_t now_ms);
uint64_t sogel_get_time_ms(void);
void     sogel_sleep_us(uint64_t microseconds);
void     sogel_render(void);

#ifdef SOGEL_IMPLEMENTATION

#include <assert.h>
#include <stdio.h>
#include <string.h>

#ifdef LOGCIE
static const char *logcie_module = "sogel";
#endif

#define SOGEL_ASSERT(cond, msg)                     \
  do {                                              \
    if (!(cond)) {                                  \
      SOGEL_LOG_FATAL("Assertion failed: %s", msg); \
      assert((cond) && (msg));                      \
    }                                               \
  } while (0)

static uint16_t sogel_width          = 80;
static uint16_t sogel_height         = 24;
static uint32_t sogel_fps            = 60;
static uint64_t sogel_frame_delay_us = 16667;

static char sogel_buffer[SOGEL_MAX_WIDTH * SOGEL_MAX_HEIGHT + 1];

static Object *sogel_objects[SOGEL_MAX_OBJECTS];
static size_t  sogel_objects_count = 0;

void sogel_set_fps(uint32_t fps) {
  SOGEL_ASSERT(fps > 0, "FPS must be greater than 0");
  sogel_fps            = fps;
  sogel_frame_delay_us = UINT64_C(1000000) / fps;
  SOGEL_LOG_DEBUG("FPS set to %u (frame delay %lu us)", fps, sogel_frame_delay_us);
}

uint32_t sogel_get_fps(void) {
  return sogel_fps;
}

uint64_t sogel_get_frame_delay_us(void) {
  return sogel_frame_delay_us;
}

void sogel_set_size(uint16_t width, uint16_t height) {
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

uint16_t sogel_get_width(void) {
  return sogel_width;
}

uint16_t sogel_get_height(void) {
  return sogel_height;
}

void sogel_hide_cursor(void) {
  printf(SOGEL_ANSI_CURSOR_HIDE);
  fflush(stdout);
  SOGEL_LOG_DEBUG("Cursor hidden");
}

void sogel_show_cursor(void) {
  printf(SOGEL_ANSI_CURSOR_SHOW);
  fflush(stdout);
  SOGEL_LOG_DEBUG("Cursor shown");
}

void sogel_clear(void) {
  memset(sogel_buffer, ' ', (size_t)sogel_width * sogel_height);
  sogel_buffer[sogel_width * sogel_height] = '\0';
  SOGEL_LOG_VERBOSE("Buffer cleared (%ux%u)", sogel_width, sogel_height);
}

char *sogel_at(uint16_t x, uint16_t y) {
  SOGEL_ASSERT(x < sogel_width && y < sogel_height, "Coordinates out of bounds");
  return sogel_buffer + (y * sogel_width + x);
}

void sogel_add_object(Object *obj) {
  SOGEL_ASSERT(obj != NULL, "Object must not be NULL");

  if (sogel_objects_count < SOGEL_MAX_OBJECTS) {
    SOGEL_LOG_DEBUG("Object added (total: %zu)", sogel_objects_count);
    sogel_objects[sogel_objects_count++] = obj;
  } else {
    SOGEL_LOG_ERROR("Cannot add object – max objects %d reached", SOGEL_MAX_OBJECTS);
  }
}

bool sogel_update(uint64_t now_ms) {
  bool changed = false;

  for (size_t i = 0; i < sogel_objects_count; i++) {
    Object *obj = sogel_objects[i];
    SOGEL_ASSERT(obj->update != NULL, "Object update function must not be NULL");

    if (now_ms - obj->last_update >= obj->update_ms) {
      obj->update(obj);
      obj->last_update = now_ms;
      changed          = true;
    }
  }

  SOGEL_LOG_TRACE("Update: %zu objects processed, changed=%d", sogel_objects_count, changed);
  return changed;
}

// Platform-specific

uint64_t sogel_get_time_ms(void) {
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

void sogel_sleep_us(uint64_t microseconds) {
  SOGEL_LOG_TRACE("Sleeping for %lu us", microseconds);
#ifdef SOGEL_PLATFORM_UNIX
  usleep(microseconds);
#else
#error "sogel_sleep_us" is not implemented for your system yet
#endif
}

void sogel_render(void) {
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

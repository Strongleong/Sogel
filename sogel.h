#ifndef SOGEL_H
#define SOGEL_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#define SOGEL_VERSION_MAJOR         0
#define SOGEL_VERSION_MINOR         7
#define SOGEL_VERSION_RELEASE       0
#define SOGEL_VERSION_NUMBER        (SOGEL_VERSION_MAJOR * 100 * 100 + SOGEL_VERSION_MINOR * 100 + SOGEL_VERSION_RELEASE)
#define SOGEL_VERSION_FULL          SOGEL_VERSION_MAJOR.SOGEL_VERSION_MINOR.SOGEL_VERSION_RELEASE
#define SOGEL_QUOTE(str)            #str
#define SOGEL_EXPAND_AND_QUOTE(str) SOGEL_QUOTE(str)
#define SOGEL_VERSION_STRING        SOGEL_EXPAND_AND_QUOTE(SOGEL_VERSION_FULL)

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
#define SOGEL_PLATFORM_UNIX
#include <sys/time.h>
#include <sys/uio.h>
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

#ifndef SOGEL_MAX_EVENTS
#define SOGEL_MAX_EVENTS 256
#endif

#ifndef SOGEL_INPUT_BUFFER_SIZE
#define SOGEL_INPUT_BUFFER_SIZE 32
#endif

#ifndef SOGEL_RESIZE_DEBOUNCE_MS
#define SOGEL_RESIZE_DEBOUNCE_MS 50
#endif

#define SOGEL_BACKEND_TERMINAL 0

#ifndef SOGEL_BACKEND
#define SOGEL_BACKEND SOGEL_BACKEND_TERMINAL
#endif

#if SOGEL_BACKEND == SOGEL_BACKEND_TERMINAL
#define SOGEL_ANSI_CLEAR_SCREEN    "\033[2J"
#define SOGEL_ANSI_HOME            "\033[H"
#define SOGEL_ANSI_CURSOR_HIDE     "\033[?25l"
#define SOGEL_ANSI_CURSOR_SHOW     "\033[?25h"
#define SOGEL_ANSI_MOUSE_TRACK_ON  "\033[?1003h"
#define SOGEL_ANSI_MOUSE_TRACK_OFF "\033[?1003l"
#define SOGEL_ANSI_SGR_ON          "\033[?1006h"
#define SOGEL_ANSI_SGR_OFF         "\033[?1006l"
#define SOGEL_ANSI_ESC             27
#define SOGEL_ANSI_CSI             '['
#define SOGEL_ANSI_MOUSE_PREFIX    '<'
#define SOGEL_ANSI_MOUSE_PRESS     'M'
#define SOGEL_ANSI_MOUSE_RELEASE   'm'
#else
#error "Only Unix+Term is supported"
#endif

// NOTE: For now for terminal backend there is no difference between lowercase and uppercase letters
#if SOGEL_BACKEND == SOGEL_BACKEND_TERMINAL
#define SOGEL_KEY_UP           256
#define SOGEL_KEY_DOWN         257
#define SOGEL_KEY_LEFT         258
#define SOGEL_KEY_RIGHT        259
#define SOGEL_KEY_ESC          27
#define SOGEL_KEY_ENTER        '\n'
#define SOGEL_KEY_BACKSPACE    8
#define SOGEL_KEY_TAB          9
#define SOGEL_KEY_RETURN       13
#define SOGEL_KEY_A            'A'
#define SOGEL_KEY_B            'B'
#define SOGEL_KEY_C            'C'
#define SOGEL_KEY_D            'D'
#define SOGEL_KEY_E            'E'
#define SOGEL_KEY_F            'F'
#define SOGEL_KEY_G            'G'
#define SOGEL_KEY_H            'H'
#define SOGEL_KEY_I            'I'
#define SOGEL_KEY_J            'J'
#define SOGEL_KEY_K            'K'
#define SOGEL_KEY_L            'L'
#define SOGEL_KEY_M            'M'
#define SOGEL_KEY_N            'N'
#define SOGEL_KEY_O            'O'
#define SOGEL_KEY_P            'P'
#define SOGEL_KEY_Q            'Q'
#define SOGEL_KEY_R            'R'
#define SOGEL_KEY_S            'S'
#define SOGEL_KEY_T            'T'
#define SOGEL_KEY_U            'U'
#define SOGEL_KEY_V            'V'
#define SOGEL_KEY_W            'W'
#define SOGEL_KEY_X            'X'
#define SOGEL_KEY_Y            'Y'
#define SOGEL_KEY_Z            'Z'
#define SOGEL_KEY_0            '0'
#define SOGEL_KEY_1            '1'
#define SOGEL_KEY_2            '2'
#define SOGEL_KEY_3            '3'
#define SOGEL_KEY_4            '4'
#define SOGEL_KEY_5            '5'
#define SOGEL_KEY_6            '6'
#define SOGEL_KEY_7            '7'
#define SOGEL_KEY_8            '8'
#define SOGEL_KEY_9            '9'
#define SOGEL_KEY_SPACE        ' '
#define SOGEL_KEY_EXCLAIM      '!'
#define SOGEL_KEY_QUOTE        '"'
#define SOGEL_KEY_HASH         '#'
#define SOGEL_KEY_DOLLAR       '$'
#define SOGEL_KEY_PERCENT      '%'
#define SOGEL_KEY_AMPERSAND    '&'
#define SOGEL_KEY_SINGLEQUOTE  '\''
#define SOGEL_KEY_LEFTPAREN    '('
#define SOGEL_KEY_RIGHTPAREN   ')'
#define SOGEL_KEY_ASTERISK     '*'
#define SOGEL_KEY_PLUS         '+'
#define SOGEL_KEY_COMMA        ','
#define SOGEL_KEY_MINUS        '-'
#define SOGEL_KEY_DOT          '.'
#define SOGEL_KEY_SLASH        '/'
#define SOGEL_KEY_COLON        ':'
#define SOGEL_KEY_SEMICOLON    ';'
#define SOGEL_KEY_LESS         '<'
#define SOGEL_KEY_EQUAL        '='
#define SOGEL_KEY_GREATER      '>'
#define SOGEL_KEY_QUESTION     '?'
#define SOGEL_KEY_AT           '@'
#define SOGEL_KEY_LEFTBRACKET  '['
#define SOGEL_KEY_RIGHTBRACKET ']'
#define SOGEL_KEY_BACKSLASH    '\\'
#define SOGEL_KEY_CARET        '^'
#define SOGEL_KEY_UNDERSCORE   '_'
#define SOGEL_KEY_BACKQUOTE    '`'
#define SOGEL_KEY_LEFTBRACE    '{'
#define SOGEL_KEY_RIGHTBRACE   '}'
#define SOGEL_KEY_PIPE         '|'
#define SOGEL_KEY_TILDE        '~'
#define SOGEL_MOUSE_LEFT       1
#define SOGEL_MOUSE_RIGHT      2
#define SOGEL_MOUSE_MIDDLE     3
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
typedef void(ObjectUpdateFn)(Object *obj, uint64_t delta_ms);
typedef void(ObjectDrawFn)(Object *obj, uint64_t delta_ms);

struct Object {
  uint16_t        x;
  uint16_t        y;
  ObjectUpdateFn *update;
  ObjectDrawFn   *draw;
};

typedef struct SogelTimer {
  uint32_t interval_ms;
  uint64_t last_time;
} SogelTimer;

typedef enum SogelEventType {
  SOGEL_EVENT_NONE = 0,

  SOGEL_EVENT_QUIT,
  SOGEL_EVENT_RESIZE,

  SOGEL_EVENT_KEY_DOWN,
  SOGEL_EVENT_KEY_UP,

  SOGEL_EVENT_CHAR,

  SOGEL_EVENT_MOUSE_MOVE,
  SOGEL_EVENT_MOUSE_DOWN,
  SOGEL_EVENT_MOUSE_UP,
  SOGEL_EVENT_MOUSE_SCROLL,

  SOGEL_EVENT_USER = 100,
} SogelEventType;

typedef struct SogelEvent {
  SogelEventType type;

  union {
    struct {
      uint16_t key;
    } key;
    struct {
      uint32_t codepoint;
    } character;
    struct {
      uint16_t width;
      uint16_t height;
    } resize;
    struct {
      int16_t x;
      int16_t y;
      uint8_t button;
    } mouse;
    struct {
      uint32_t code;
      void    *data1;
      void    *data2;
    } user;
  };
} SogelEvent;

// Lifecycle
SOGEL_DEF void     sogel_init(uint16_t width, uint16_t height, uint32_t fps);
SOGEL_DEF void     sogel_deinit(void);
// Configuration
SOGEL_DEF void     sogel_set_fps(uint32_t fps);
SOGEL_DEF uint32_t sogel_get_fps(void);
SOGEL_DEF uint64_t sogel_get_frame_delay_us(void);
SOGEL_DEF void     sogel_set_size(uint16_t width, uint16_t height);
SOGEL_DEF uint16_t sogel_get_width(void);
SOGEL_DEF uint16_t sogel_get_height(void);
// Terminal specific
SOGEL_DEF void     sogel_hide_cursor(void);
SOGEL_DEF void     sogel_show_cursor(void);
SOGEL_DEF void     sogel_clear(void);
// Drawing buffer
SOGEL_DEF void     sogel_clear_term(void);
SOGEL_DEF char    *sogel_at(uint16_t x, uint16_t y);
// Object management
SOGEL_DEF void     sogel_add_object(Object *obj);
// Game loop
SOGEL_DEF void     sogel_update(void);
SOGEL_DEF void     sogel_draw(void);
SOGEL_DEF uint64_t sogel_get_time_ms(void);
SOGEL_DEF void     sogel_sleep_us(uint64_t microseconds);
SOGEL_DEF void     sogel_render(void);
SOGEL_DEF bool     sogel_timer_elapsed(SogelTimer *t);
// Event system
SOGEL_DEF void     sogel_poll_events(void);
SOGEL_DEF bool     sogel_next_event(SogelEvent *event);
SOGEL_DEF bool     sogel_push_user_event(uint32_t code, void *data1, void *data2);
SOGEL_DEF bool     sogel_is_key_down(uint16_t key);
SOGEL_DEF bool     sogel_is_key_pressed(uint16_t key);
SOGEL_DEF bool     sogel_is_key_released(uint16_t key);
SOGEL_DEF int16_t  sogel_get_mouse_x(void);
SOGEL_DEF int16_t  sogel_get_mouse_y(void);
SOGEL_DEF bool     sogel_is_mouse_down(uint8_t button);

#ifdef SOGEL_IMPLEMENTATION

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef SOGEL_PLATFORM_UNIX
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>

typedef struct termios TermState;
#else
#error Your platfor is not supported yet
#endif

#ifdef LOGCIE
static const char *logcie_module = "sogel";
#endif

static uint16_t sogel_width          = 80;
static uint16_t sogel_height         = 24;
static uint32_t sogel_fps            = 60;
static uint64_t sogel_frame_delay_us = 16667;

static char  *sogel_buffer      = NULL;
static size_t sogel_buffer_size = 0;

static Object *sogel_objects[SOGEL_MAX_OBJECTS];
static size_t  sogel_objects_count = 0;

#define SOGEL_MAX_KEY 512
static bool sogel_keys_curr[SOGEL_MAX_KEY] = {0};
static bool sogel_keys_prev[SOGEL_MAX_KEY] = {0};

static int16_t sogel_mouse_x          = 0;
static int16_t sogel_mouse_y          = 0;
static bool    sogel_mouse_buttons[8] = {0};

static SogelEvent sogel_events[SOGEL_MAX_EVENTS];
static uint16_t   sogel_event_head = 0;
static uint16_t   sogel_event_tail = 0;

static uint64_t sogel_last_tick      = 0;
static uint64_t sogel_frame_delta_ms = 0;

static TermState sogel_orig_termios;
static bool      sogel_termios_saved = false;

static volatile bool sogel_terminal_resized = false;
static uint64_t      sogel_last_resize_ms   = 0;

static const char *sogel_event_name(SogelEventType type) {
  static const char *names[] = {
    [SOGEL_EVENT_NONE]         = "NONE",
    [SOGEL_EVENT_QUIT]         = "QUIT",
    [SOGEL_EVENT_RESIZE]       = "RESIZE",
    [SOGEL_EVENT_KEY_DOWN]     = "KEY_DOWN",
    [SOGEL_EVENT_KEY_UP]       = "KEY_UP",
    [SOGEL_EVENT_CHAR]         = "CHAR",
    [SOGEL_EVENT_MOUSE_MOVE]   = "MOUSE_MOVE",
    [SOGEL_EVENT_MOUSE_DOWN]   = "MOUSE_DOWN",
    [SOGEL_EVENT_MOUSE_UP]     = "MOUSE_UP",
    [SOGEL_EVENT_MOUSE_SCROLL] = "MOUSE_SCROLL",
    [SOGEL_EVENT_USER]         = "USER",
  };

  if (type >= 0 && type < (SogelEventType)(sizeof(names) / sizeof(names[0]))) {
    return names[type];
  }

  return "UNKNOWN";
}

static void sogel_terminal_restore(void) {
#if SOGEL_BACKEND == SOGEL_BACKEND_TERMINAL && defined(SOGEL_PLATFORM_UNIX)
  if (sogel_termios_saved) {
    tcsetattr(STDIN_FILENO, TCSANOW, &sogel_orig_termios);
  }

  printf(SOGEL_ANSI_MOUSE_TRACK_OFF);
  printf(SOGEL_ANSI_SGR_OFF);
  fflush(stdout);

  free(sogel_buffer);
  sogel_buffer = NULL;
#else
#error "sogel_terminal_restore" is not supported on your platform yet
#endif
}

static void sogel_terminal_enable_raw(void) {
#if SOGEL_BACKEND == SOGEL_BACKEND_TERMINAL && defined(SOGEL_PLATFORM_UNIX)
  if (!sogel_termios_saved) {
    int32_t rc = tcgetattr(STDIN_FILENO, &sogel_orig_termios);
    SOGEL_ASSERT(rc == 0, "tcgetattr failed – cannot save terminal settings");
    sogel_termios_saved = true;
    atexit(sogel_terminal_restore);
    SOGEL_LOG_DEBUG("Terminal settings are saved");

    printf(SOGEL_ANSI_MOUSE_TRACK_ON);
    printf(SOGEL_ANSI_SGR_ON);
    fflush(stdout);
  }

  struct termios raw = sogel_orig_termios;
  raw.c_lflag &= ~(ICANON | ECHO);

  int32_t rc = tcsetattr(STDIN_FILENO, TCSANOW, &raw);
  SOGEL_ASSERT(rc == 0, "tcgetattr failed – cannot save terminal settings");

  int32_t flags = fcntl(STDIN_FILENO, F_GETFL, 0);
  SOGEL_ASSERT(flags != -1, "fcntl F_GETFL failed");
  fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

  SOGEL_LOG_DEBUG("Raw input mode enabled");
#else
#error "sogel_terminal_enable_raw" is not supported on your platform yet
#endif
}

static void sogel_sigwinch_handler(int sig) {
#if SOGEL_BACKEND == SOGEL_BACKEND_TERMINAL && defined(SOGEL_PLATFORM_UNIX)
  (void)sig;
  sogel_terminal_resized = true;
#else
#error "sogel_sigwinch_handler" is not supported on your platform yet
#endif
}

static bool sogel_push_event(const SogelEvent *event) {
  uint32_t next = (sogel_event_tail + 1) % SOGEL_MAX_EVENTS;

  if (next == sogel_event_head) {
    SOGEL_LOG_WARN("Event queue overflow - event dropped");
    return false;
  }

  sogel_events[sogel_event_tail] = *event;
  sogel_event_tail               = next;

  SOGEL_LOG_TRACE("Event pushed: %s", sogel_event_name(event->type));
  return true;
}

static void sogel_resize_buffer(uint16_t width, uint16_t height) {
  size_t new_size   = (size_t)width * height + 1;
  char  *new_buffer = realloc(sogel_buffer, new_size);

  SOGEL_ASSERT(new_buffer != NULL, "Failed to allocate framebuffer");

  sogel_buffer      = new_buffer;
  sogel_buffer_size = new_size;

  memset(sogel_buffer, ' ', new_size - 1);
  sogel_buffer[new_size - 1] = '\0';
}

static void sogel_update_terminal_size(void) {
#ifdef SOGEL_PLATFORM_UNIX
  struct winsize ws;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
    return;
  }

  sogel_set_size(ws.ws_col, ws.ws_row);

  SogelEvent e = {
    .type = SOGEL_EVENT_RESIZE,
  };

  e.resize.width  = ws.ws_col;
  e.resize.height = ws.ws_row;

  sogel_push_event(&e);
#endif
}

static bool sogel_parse_mouse_event(const uint8_t *buf, int32_t len, int32_t *consumed) {
  const int32_t min_mouse_len = 6;

  if (len < min_mouse_len) {
    return false;
  }

  if (buf[0] != SOGEL_ANSI_ESC || buf[1] != SOGEL_ANSI_CSI || buf[2] != SOGEL_ANSI_MOUSE_PREFIX) {
    return false;
  }

  int  b, x, y;
  char type;
  int  parsed_bytes = 0;

  int parsed = sscanf((const char *)buf, "\033[<%d;%d;%d%c%n", &b, &x, &y, &type, &parsed_bytes);

  if (parsed != 4) {
    return false;
  }

  SogelEvent e = {0};

  const int8_t mouse_motion_mask   = 32;
  const int8_t mouse_button_mask   = 3;
  const int8_t mouse_button_offset = 1;

  if ((b & mouse_motion_mask) != 0) {
    e.type = SOGEL_EVENT_MOUSE_MOVE;
  } else if (type == SOGEL_ANSI_MOUSE_PRESS) {
    e.type = SOGEL_EVENT_MOUSE_DOWN;
  } else {
    e.type = SOGEL_EVENT_MOUSE_UP;
  }

  e.mouse.x      = (int16_t)x - 1;
  e.mouse.y      = (int16_t)y - 1;
  e.mouse.button = (uint8_t)(b & mouse_button_mask) + mouse_button_offset;

  sogel_mouse_x = e.mouse.x;
  sogel_mouse_y = e.mouse.y;

  if (e.type == SOGEL_EVENT_MOUSE_DOWN) {
    sogel_mouse_buttons[e.mouse.button] = true;
  }

  else if (e.type == SOGEL_EVENT_MOUSE_UP) {
    sogel_mouse_buttons[e.mouse.button] = false;
  }

  sogel_push_event(&e);
  *consumed = parsed_bytes;

  return true;
}

static bool sogel_parse_arrow_key(const uint8_t *buf, int32_t len, int32_t *consumed) {
  const int32_t arrow_len = 3;

  if (len < arrow_len) {
    return false;
  }

  if (buf[0] != SOGEL_ANSI_ESC || buf[1] != SOGEL_ANSI_CSI) {
    return false;
  }

  uint16_t key = 0;

  switch (buf[2]) {
    case 'A': key = SOGEL_KEY_UP; break;
    case 'B': key = SOGEL_KEY_DOWN; break;
    case 'C': key = SOGEL_KEY_RIGHT; break;
    case 'D': key = SOGEL_KEY_LEFT; break;
    default:  return false;
  }

  sogel_keys_curr[key] = true;

  SogelEvent e = {.type = SOGEL_EVENT_KEY_DOWN, .key.key = key};
  sogel_push_event(&e);
  *consumed = arrow_len;

  return true;
}

static void sogel_process_input_buffer(const uint8_t *buf, int32_t n) {
  for (int32_t i = 0; i < n; i++) {
    if (
      buf[i] == SOGEL_ANSI_ESC &&
      i + 1 < n &&
      buf[i + 1] == SOGEL_ANSI_CSI
    ) {
      int32_t consumed = 0;

      if (
        sogel_parse_mouse_event(&buf[i], n - i, &consumed) ||
        sogel_parse_arrow_key(&buf[i], n - i, &consumed)
      ) {
        i += consumed - 1;
        continue;
      }

      i++;
      continue;
    }

    uint16_t ch = buf[i];
    if (ch < SOGEL_MAX_KEY) {
      sogel_keys_curr[ch] = true;

      SogelEvent e_down = {.type = SOGEL_EVENT_KEY_DOWN, .key.key = ch};
      sogel_push_event(&e_down);

      SogelEvent e_char = {.type = SOGEL_EVENT_CHAR, .character.codepoint = ch};
      sogel_push_event(&e_char);
    }
  }
}

static void sogel_generate_key_up_events(void) {
  for (uint16_t k = 0; k < SOGEL_MAX_KEY; k++) {
    if (!sogel_keys_curr[k] && sogel_keys_prev[k]) {
      SogelEvent e = {.type = SOGEL_EVENT_KEY_UP, .key.key = k};
      sogel_push_event(&e);
    }
  }
}

SOGEL_DEF void sogel_init(uint16_t width, uint16_t height, uint32_t fps) {
  sogel_set_fps(fps);
  sogel_set_size(width, height);
  SOGEL_LOG_INFO("Sogel initialising (backend %d)", SOGEL_BACKEND);

#if SOGEL_BACKEND == SOGEL_BACKEND_TERMINAL
  sogel_clear_term();
  sogel_hide_cursor();
#endif

#ifdef SOGEL_PLATFORM_UNIX
  sogel_terminal_enable_raw();
  signal(SIGWINCH, sogel_sigwinch_handler);
  sogel_update_terminal_size();
#else
#error "sogel_init: platform input init not implemented"
#endif

  sogel_last_tick = sogel_get_time_ms();
  SOGEL_LOG_DEBUG("Init complete (clock initialised)");
}

SOGEL_DEF void sogel_deinit(void) {
  SOGEL_LOG_INFO("Sogel shutting down");

#ifdef SOGEL_PLATFORM_UNIX
  sogel_terminal_restore();
#endif

  free(sogel_buffer);
  sogel_buffer = NULL;

  SOGEL_LOG_DEBUG("Deinit complete");
}

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

  sogel_width  = width;
  sogel_height = height;

  sogel_resize_buffer(width, height);

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
#if SOGEL_BACKEND == SOGEL_BACKEND_TERMINAL && defined(SOGEL_PLATFORM_UNIX)
  printf(SOGEL_ANSI_CURSOR_HIDE);
  fflush(stdout);
  SOGEL_LOG_DEBUG("Cursor hidden");
#else
#error "sogel_hide_cursor" is not defined for your platform yet
#endif
}

SOGEL_DEF void sogel_show_cursor(void) {
#if SOGEL_BACKEND == SOGEL_BACKEND_TERMINAL && defined(SOGEL_PLATFORM_UNIX)
  printf(SOGEL_ANSI_CURSOR_SHOW);
  fflush(stdout);
  SOGEL_LOG_DEBUG("Cursor shown");
#else
#error "sogel_show_cursor" is not defined for your platform yet
#endif
}

SOGEL_DEF void sogel_clear_term(void) {
#if SOGEL_BACKEND == SOGEL_BACKEND_TERMINAL && defined(SOGEL_PLATFORM_UNIX)
  printf(SOGEL_ANSI_CLEAR_SCREEN);
  fflush(stdout);
  SOGEL_LOG_DEBUG("Terminal was cleared");
#else
#error "sogel_clear_term" is not defined for your platform yet
#endif
}

SOGEL_DEF void sogel_clear(void) {
  memset(sogel_buffer, ' ', (size_t)sogel_width * sogel_height);
  sogel_buffer[sogel_width * sogel_height] = '\0';
  SOGEL_LOG_TRACE("Buffer cleared (%ux%u)", sogel_width, sogel_height);
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

SOGEL_DEF void sogel_update(void) {
  uint64_t now         = sogel_get_time_ms();
  sogel_frame_delta_ms = now - sogel_last_tick;
  sogel_last_tick      = now;

  for (size_t i = 0; i < sogel_objects_count; i++) {
    Object *obj = sogel_objects[i];

    if (obj->update) {
      obj->update(obj, sogel_frame_delta_ms);
      SOGEL_LOG_TRACE("Object %zu update() called", i);
    }
  }
}

SOGEL_DEF void sogel_draw(void) {
  for (size_t i = 0; i < sogel_objects_count; i++) {
    Object *obj = sogel_objects[i];

    if (obj->draw) {
      obj->draw(obj, sogel_frame_delta_ms);
      SOGEL_LOG_TRACE("Object %zu draw() called", i);
    }
  }
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

  const uint16_t h = sogel_height;
  const uint16_t w = sogel_width;

  /* iovec layout: ANSI_HOME | row0 | '\n' | row1 | '\n' | ... | row_{h-1} (no '\n') */
  uint16_t     iov_count = 1 + 2 * (uint16_t)h;
  struct iovec iov[1 + 2 * SOGEL_MAX_HEIGHT];
  uint16_t     idx = 0;

  iov[idx].iov_base = (void *)SOGEL_ANSI_HOME;
  iov[idx].iov_len  = 3;
  idx++;

  for (uint16_t y = 0; y < h - 1; y++) {
    iov[idx].iov_base = sogel_buffer + (size_t)y * w;
    iov[idx].iov_len  = w;
    idx++;

    iov[idx].iov_base = (void *)"\n";
    iov[idx].iov_len  = 1;
    idx++;
  }

  iov[idx].iov_base = sogel_buffer + (size_t)(h - 1) * w;
  iov[idx].iov_len  = w;

  iov_count = idx;

  ssize_t total = writev(STDOUT_FILENO, iov, iov_count);

  if (total == -1) {
    SOGEL_LOG_ERROR("writev failed: %s", strerror(errno));
  } else {
    SOGEL_LOG_TRACE("Frame rendered, %zd bytes written", total);
  }

  fflush(stdout);
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

SOGEL_DEF void sogel_poll_events(void) {
  memcpy(sogel_keys_prev, sogel_keys_curr, sizeof(sogel_keys_prev));
  memset(sogel_keys_curr, 0, sizeof(sogel_keys_curr));

  if (sogel_terminal_resized) {
    uint64_t now = sogel_get_time_ms();

    if (now - sogel_last_resize_ms > SOGEL_RESIZE_DEBOUNCE_MS) {
      sogel_terminal_resized = false;
      sogel_last_resize_ms   = now;
      sogel_update_terminal_size();
    }
  }

#ifdef SOGEL_PLATFORM_UNIX
  SOGEL_ASSERT(sogel_termios_saved, "Input not set up – call sogel_init() first");

  uint8_t buf[SOGEL_INPUT_BUFFER_SIZE];
  int32_t n = read(STDIN_FILENO, buf, sizeof(buf));

  // no data = not an error
  if (n == -1 && errno != EAGAIN) {
    SOGEL_LOG_ERROR("read from stdin failed: %s", strerror(errno));
  } else if (n > 0) {
    sogel_process_input_buffer(buf, n);
    SOGEL_LOG_TRACE("Processed %d input bytes", n);
  }

  sogel_generate_key_up_events();
#else
#error "sogel_poll_events not implemented"
#endif
}

SOGEL_DEF bool sogel_next_event(SogelEvent *event) {
  SOGEL_ASSERT(event != NULL, "Event must not be NULL");

  if (sogel_event_head == sogel_event_tail) {
    return false;
  }

  *event = sogel_events[sogel_event_head];

  sogel_event_head = (sogel_event_head + 1) % SOGEL_MAX_EVENTS;
  return true;
}

SOGEL_DEF bool sogel_push_user_event(uint32_t code, void *data1, void *data2) {
  SogelEvent e = {
    .type = SOGEL_EVENT_USER,
  };

  e.user.code  = code;
  e.user.data1 = data1;
  e.user.data2 = data2;

  return sogel_push_event(&e);
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

SOGEL_DEF int16_t sogel_get_mouse_x(void) {
  return sogel_mouse_x;
}

SOGEL_DEF int16_t sogel_get_mouse_y(void) {
  return sogel_mouse_y;
}

SOGEL_DEF bool sogel_is_mouse_down(uint8_t button) {
  return (button < 8) ? sogel_mouse_buttons[button] : false;
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

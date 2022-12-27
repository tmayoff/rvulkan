#ifndef KEYCODES_HPP
#define KEYCODES_HPP

enum class Key {
  A = 4,
  B = 5,
  C = 6,
  D = 7,
  E = 8,
  F = 9,
  G = 10,
  H = 11,
  I = 12,
  J = 13,
  K = 14,
  L = 15,
  M = 16,
  N = 17,
  O = 18,
  P = 19,
  Q = 20,
  R = 21,
  S = 22,
  T = 23,
  U = 24,
  V = 25,
  W = 26,
  X = 27,
  Y = 28,
  Z = 29,

  D1 = 30,
  D2 = 31,
  D3 = 32,
  D4 = 33,
  D5 = 34,
  D6 = 35,
  D7 = 36,
  D8 = 37,
  D9 = 38,
  D0 = 39,

  RETURN = 40,
  ESCAPE = 41,
  BACKSPACE = 42,
  TAB = 43,
  SPACE = 44,

  MINUS = 45,
  EQUALS = 46,
  LEFTBRACKET = 47,
  RIGHTBRACKET = 48,
  BACKSLASH = 49,
  NONUSHASH = 50,
  SEMICOLON = 51,
  APOSTROPHE = 52,
  GRAVE = 53,
  COMMA = 54,
  PERIOD = 55,
  SLASH = 56,

  CAPSLOCK = 57,

  F1 = 58,
  F2 = 59,
  F3 = 60,
  F4 = 61,
  F5 = 62,
  F6 = 63,
  F7 = 64,
  F8 = 65,
  F9 = 66,
  F10 = 67,
  F11 = 68,
  F12 = 69,

  PRINTSCREEN = 70,
  SCROLLLOCK = 71,
  PAUSE = 72,
  INSERT = 73, /**< insert on PC, help on some Mac keyboards (but
                                 does send code 73, not 117) */
  HOME = 74,
  PAGEUP = 75,
  DELETE = 76,
  END = 77,
  PAGEDOWN = 78,
  RIGHT = 79,
  LEFT = 80,
  DOWN = 81,
  UP = 82,

  NUMLOCKCLEAR = 83, /**< num lock on PC, clear on Mac keyboards
                      */
  KP_DIVIDE = 84,
  KP_MULTIPLY = 85,
  KP_MINUS = 86,
  KP_PLUS = 87,
  KP_ENTER = 88,
  KP_1 = 89,
  KP_2 = 90,
  KP_3 = 91,
  KP_4 = 92,
  KP_5 = 93,
  KP_6 = 94,
  KP_7 = 95,
  KP_8 = 96,
  KP_9 = 97,
  KP_0 = 98,
  KP_PERIOD = 99,

  NONUSBACKSLASH = 100, /**< This is the additional key that ISO
                         *   keyboards have over ANSI ones,
                         *   located between left shift and Y.
                         *   Produces GRAVE ACCENT and TILDE in a
                         *   US or UK Mac layout, REVERSE SOLIDUS
                         *   (backslash) and VERTICAL LINE in a
                         *   US or UK Windows layout, and
                         *   LESS-THAN SIGN and GREATER-THAN SIGN
                         *   in a Swiss German, German, or French
                         *   layout. */
  APPLICATION = 101,    /**< windows contextual menu, compose */
  POWER = 102,          /**< The USB document says this is a status flag,
                         *   not a physical key - but some Mac keyboards
                         *   do have a power key. */
  KP_EQUALS = 103,
  F13 = 104,
  F14 = 105,
  F15 = 106,
  F16 = 107,
  F17 = 108,
  F18 = 109,
  F19 = 110,
  F20 = 111,
  F21 = 112,
  F22 = 113,
  F23 = 114,
  F24 = 115,
  EXECUTE = 116,
  HELP = 117,
  MENU = 118,
  SELECT = 119,
  STOP = 120,
  AGAIN = 121, /**< redo */
  UNDO = 122,
  CUT = 123,
  COPY = 124,
  PASTE = 125,
  FIND = 126,
  MUTE = 127,
  VOLUMEUP = 128,
  VOLUMEDOWN = 129,

  LCTRL = 224,
  LSHIFT = 225,
  LALT = 226, /**< alt, option */
  LGUI = 227, /**< windows, command (apple), meta */
  RCTRL = 228,
  RSHIFT = 229,
  RALT = 230, /**< alt gr, option */
  RGUI = 231, /**< windows, command (apple), meta */

};

#endif  // KEYCODES_HPP

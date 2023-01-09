#ifndef KEYCODES_HPP
#define KEYCODES_HPP

#include <imgui.h>

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

  KP_EQUALS = 103,

  MENU = 118,

  LCTRL = 224,
  LSHIFT = 225,
  LALT = 226, /**< alt, option */
  RCTRL = 228,
  RSHIFT = 229,
  RALT = 230, /**< alt gr, option */

};

inline static ImGuiKey KeyCodeKeycodeToImGuiKey(Key keycode) {
  switch (keycode) {
    case Key::TAB: return ImGuiKey_Tab;
    case Key::A: return ImGuiKey_A;
    case Key::B: return ImGuiKey_B;
    case Key::C: return ImGuiKey_C;
    case Key::D: return ImGuiKey_D;
    case Key::E: return ImGuiKey_E;
    case Key::F: return ImGuiKey_F;
    case Key::G: return ImGuiKey_G;
    case Key::H: return ImGuiKey_H;
    case Key::I: return ImGuiKey_I;
    case Key::J: return ImGuiKey_J;
    case Key::K: return ImGuiKey_K;
    case Key::L: return ImGuiKey_L;
    case Key::M: return ImGuiKey_M;
    case Key::N: return ImGuiKey_N;
    case Key::O: return ImGuiKey_O;
    case Key::P: return ImGuiKey_P;
    case Key::Q: return ImGuiKey_Q;
    case Key::R: return ImGuiKey_R;
    case Key::S: return ImGuiKey_S;
    case Key::T: return ImGuiKey_T;
    case Key::U: return ImGuiKey_U;
    case Key::V: return ImGuiKey_V;
    case Key::W: return ImGuiKey_W;
    case Key::X: return ImGuiKey_X;
    case Key::Y: return ImGuiKey_Y;
    case Key::Z: return ImGuiKey_Z;
    case Key::D1: return ImGuiKey_1;
    case Key::D2: return ImGuiKey_2;
    case Key::D3: return ImGuiKey_3;
    case Key::D4: return ImGuiKey_4;
    case Key::D5: return ImGuiKey_5;
    case Key::D6: return ImGuiKey_6;
    case Key::D7: return ImGuiKey_7;
    case Key::D8: return ImGuiKey_8;
    case Key::D9: return ImGuiKey_9;
    case Key::D0: return ImGuiKey_0;
    case Key::RETURN: return ImGuiKey_Enter;
    case Key::ESCAPE: return ImGuiKey_Escape;
    case Key::BACKSPACE: return ImGuiKey_Backspace;
    case Key::SPACE: return ImGuiKey_Space;
    case Key::MINUS: return ImGuiKey_Minus;
    case Key::EQUALS: return ImGuiKey_Equal;
    case Key::LEFTBRACKET: return ImGuiKey_LeftBracket;
    case Key::RIGHTBRACKET: return ImGuiKey_RightBracket;
    case Key::BACKSLASH: return ImGuiKey_Backslash;
    case Key::SEMICOLON: return ImGuiKey_Semicolon;
    case Key::APOSTROPHE: return ImGuiKey_Apostrophe;
    case Key::GRAVE: return ImGuiKey_GraveAccent;
    case Key::COMMA: return ImGuiKey_Comma;
    case Key::PERIOD: return ImGuiKey_Period;
    case Key::SLASH: return ImGuiKey_Slash;
    case Key::CAPSLOCK: return ImGuiKey_CapsLock;
    case Key::F1: return ImGuiKey_F1;
    case Key::F2: return ImGuiKey_F2;
    case Key::F3: return ImGuiKey_F3;
    case Key::F4: return ImGuiKey_F4;
    case Key::F5: return ImGuiKey_F5;
    case Key::F6: return ImGuiKey_F6;
    case Key::F7: return ImGuiKey_F7;
    case Key::F8: return ImGuiKey_F8;
    case Key::F9: return ImGuiKey_F9;
    case Key::F10: return ImGuiKey_F10;
    case Key::F11: return ImGuiKey_F11;
    case Key::F12: return ImGuiKey_F12;
    case Key::PRINTSCREEN: return ImGuiKey_PrintScreen;
    case Key::SCROLLLOCK: return ImGuiKey_ScrollLock;
    case Key::PAUSE: return ImGuiKey_Pause;
    case Key::INSERT: return ImGuiKey_Insert;
    case Key::HOME: return ImGuiKey_Home;
    case Key::PAGEUP: return ImGuiKey_PageUp;
    case Key::DELETE: return ImGuiKey_Delete;
    case Key::END: return ImGuiKey_End;
    case Key::PAGEDOWN: return ImGuiKey_PageDown;
    case Key::RIGHT: return ImGuiKey_RightArrow;
    case Key::LEFT: return ImGuiKey_LeftArrow;
    case Key::DOWN: return ImGuiKey_DownArrow;
    case Key::UP: return ImGuiKey_UpArrow;
    case Key::NUMLOCKCLEAR: return ImGuiKey_NumLock;
    case Key::KP_DIVIDE: return ImGuiKey_KeypadDivide;
    case Key::KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
    case Key::KP_MINUS: return ImGuiKey_KeypadSubtract;
    case Key::KP_PLUS: return ImGuiKey_KeypadAdd;
    case Key::KP_ENTER: return ImGuiKey_KeypadEnter;
    case Key::KP_1: return ImGuiKey_1;
    case Key::KP_2: return ImGuiKey_2;
    case Key::KP_3: return ImGuiKey_3;
    case Key::KP_4: return ImGuiKey_4;
    case Key::KP_5: return ImGuiKey_5;
    case Key::KP_6: return ImGuiKey_6;
    case Key::KP_7: return ImGuiKey_7;
    case Key::KP_8: return ImGuiKey_8;
    case Key::KP_9: return ImGuiKey_9;
    case Key::KP_0: return ImGuiKey_0;
    case Key::KP_PERIOD: return ImGuiKey_KeypadDecimal;
    case Key::KP_EQUALS: return ImGuiKey_KeypadEqual;
    case Key::MENU: return ImGuiKey_Menu;
    case Key::LCTRL: return ImGuiKey_LeftCtrl;
    case Key::LSHIFT: return ImGuiKey_LeftShift;
    case Key::LALT: return ImGuiKey_LeftAlt;
    case Key::RCTRL: return ImGuiKey_RightCtrl;
    case Key::RSHIFT: return ImGuiKey_RightShift;
    case Key::RALT: return ImGuiKey_RightAlt;
  }
  return ImGuiKey_None;
}

#endif  // KEYCODES_HPP

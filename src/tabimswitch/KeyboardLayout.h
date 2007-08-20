#ifndef __KEYBOARD_LAYOUT_H__
#define __KEYBOARD_LAYOUT_H__

#include <string>

struct DefaultKeyboardLayout
{
  static std::wstring getCurrent(void);
  static bool setCurrent(std::wstring const& kbl);
  static char const* getName(void);
};

struct MultilingualKeyboardLayout
{
  static std::wstring getCurrent(void);
  static bool setCurrent(std::wstring const& kbl);
  static char const* getName(void);
};

#endif //__KEYBOARD_LAYOUT_H__

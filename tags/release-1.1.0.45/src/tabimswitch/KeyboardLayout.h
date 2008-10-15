#ifndef __KEYBOARD_LAYOUT_H__
#define __KEYBOARD_LAYOUT_H__

#include <string>

class KeyboardLayout
{
public:
  virtual bool init(void) = 0;
  virtual std::wstring getCurrent(void) = 0;
  virtual void setCurrent(std::wstring const& kbl) = 0;
  virtual bool isMultilingual(void) const = 0;

  virtual ~KeyboardLayout(void) {}

  static KeyboardLayout* createDefault(void);
  static KeyboardLayout* createMultilingual(void);
  static KeyboardLayout* createByEnvironment(void);

  static bool isMultilingualEnv(void);
  static bool hasMSPY() { return sm_hasMSPY; }

protected:
  KeyboardLayout(void) {}

  static bool sm_hasMSPY;
};

#endif //__KEYBOARD_LAYOUT_H__

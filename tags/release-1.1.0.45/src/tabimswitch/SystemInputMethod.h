#ifndef _SYSTEM_INPUT_METHOD_H_
#define _SYSTEM_INPUT_METHOD_H_

#include <memory>
#include <string>

class KeyboardLayout;
class InputMethodContext;

class SystemInputMethod
{
public:
  static SystemInputMethod& get(void);

public:
  enum KeyboardLayoutManagerType
  {
    AutoKeyboardLayoutManager, 
    DefaultKeyboardLayoutManager,
    MultilingualKeyboardLayoutManager,
  };

  bool isMultilingualEnvironment(void) const;

  void useKeyboardLayoutManager(KeyboardLayoutManagerType type);
  KeyboardLayoutManagerType getKeyboardLayoutManagerType(void) const;

  void init(void);

  KeyboardLayout& getKeyboardLayout(void) { return *m_keyboardLayout; }
  InputMethodContext& getInputMethodContext(void) { return *m_inputMethodContext; }

  std::wstring getInputMethodList(void) const;
  wchar_t getInputMethodListSeparator(void) const;

private:
  SystemInputMethod(void);
  ~SystemInputMethod(void);

private:
  std::auto_ptr<KeyboardLayout> m_keyboardLayout;
  std::auto_ptr<InputMethodContext> m_inputMethodContext;

  KeyboardLayoutManagerType m_keyboardLayoutManagerType;

  static const wchar_t SEPARATOR=L';';
};

#endif // _SYSTEM_INPUT_METHOD_H_

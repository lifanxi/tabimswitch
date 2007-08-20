#ifndef _INPUT_METHOD_CONTEXT_H_
#define _INPUT_METHOD_CONTEXT_H_

#include <windows.h>
#include "Uncopyable.h"

class InputMethodContext
  : Uncopyable
{
public:
  static InputMethodContext& get(void);

protected:
  InputMethodContext(void);
  ~InputMethodContext(void);

public:
  bool init(void);

  void enable(void);
  void disable(void);
  bool isEnabled(void);

private:
  bool enableWindowIME(HWND hwnd);
  bool disableWindowIME(HWND hwnd);
  bool getWindowIMEStatus(HWND hwnd);

private:
  typedef bool (InputMethodContext::*pfxForeachCallback)(HWND hWnd);
  struct EnumCallbackParam
  {
    InputMethodContext* pThis;
    pfxForeachCallback pCallback;
  };
  void forEachFirefoxWindow(pfxForeachCallback callback);
  static BOOL CALLBACK enumWndProc(HWND hwnd, LPARAM lParam);

private:
  bool m_enable;
  HIMC m_enabledIMC;
};

#endif // _INPUT_METHOD_CONTEXT_H_

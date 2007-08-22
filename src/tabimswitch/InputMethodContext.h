#ifndef _INPUT_METHOD_CONTEXT_H_
#define _INPUT_METHOD_CONTEXT_H_

#include <windows.h>
#include "Uncopyable.h"
#include <set>

class InputMethodContext
  : public Uncopyable
{
public:
  InputMethodContext(void);
  ~InputMethodContext(void);

public:
  bool init(void);

  void enable(void);
  void disable(void);
  bool isEnabled(void);

  void getIMEMode(DWORD& convMode, DWORD& sentMode, BOOL& englishMode, BOOL& isSentModeNone);
  void setIMEMode(DWORD convMode, DWORD sentMode, BOOL englishMode, BOOL isSentModeNone);

private:
  enum SystemLanguageID
  {
    LID_TRADITIONAL_CHINESE = 0x0404,
    LID_JAPANESE = 0x0411,
    LID_KOREAN = 0x0412,
    LID_SIMPLIFIED_CHINESE = 0x0804
  };

  enum LanguageID
  {
    DEFAULT,				
    TRADITIONAL_CHINESE,	
    JAPANESE,
    KOREAN,
    SIMPLIFIED_CHINESE
  };

  bool enableWindowIME(HWND hwnd);
  bool disableWindowIME(HWND hwnd);
  bool fillAppIMC(HWND hwnd);
  bool getIMEWindow(HWND hwnd);

  LanguageID getLanguage(void);

  void fixupGotIMEMode(DWORD& convMode, DWORD& sentMode, BOOL& englishMode, BOOL& isSentModeNone);
  void fixupSetIMEMode(DWORD& convMode, DWORD& sentMode, BOOL englishMode, BOOL isSentModeNone);

  HIMC getIMC(void);
  bool refreshIMC(void);

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
  HIMC m_savedIMC; // The saved IMC when disable IME.
  std::set<HIMC> m_appIMC;
  HWND m_hwndIME; // the IME window
};

#endif // _INPUT_METHOD_CONTEXT_H_

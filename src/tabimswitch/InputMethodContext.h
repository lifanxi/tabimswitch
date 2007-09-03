#ifndef _INPUT_METHOD_CONTEXT_H_
#define _INPUT_METHOD_CONTEXT_H_

#include <windows.h>
#include "Uncopyable.h"
#include <set>

// #define _USE_IME_FLAGS_FIXUP

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

#ifndef _USE_IME_FLAGS_FIXUP

  void getIMEMode(DWORD& convMode, DWORD& sentMode);
  void setIMEMode(DWORD convMode, DWORD sentMode);

  BOOL getOpenStatus(void);
  void setOpenStatus(bool status);

#else // _USE_IME_FLAGS_FIXUP

  void getIMEMode(DWORD& convMode, DWORD& sentMode, BOOL& englishMode, BOOL& isSentModeNone);
  void setIMEMode(DWORD convMode, DWORD sentMode, BOOL englishMode, BOOL isSentModeNone);

#endif // _USE_IME_FLAGS_FIXUP

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

  LanguageID getLanguage(void);

#ifdef _USE_IME_FLAGS_FIXUP
  void fixupGotIMEMode(DWORD& convMode, DWORD& sentMode, BOOL& englishMode, BOOL& isSentModeNone);
  void fixupSetIMEMode(DWORD& convMode, DWORD& sentMode, BOOL englishMode, BOOL isSentModeNone);
#endif

  bool hasSavedIMC(void);

  bool enableWindowIME(HWND hwnd);
  bool disableWindowIME(HWND hwnd);

  bool setWindowIMEMode(HWND hwnd);
  bool setWindowIMEOpenStatus(HWND hwnd);
  bool getWindowIMEMode(HWND hwnd);
  bool getWindowIMEOpenStatus(HWND hwnd);

private:
  typedef bool (InputMethodContext::*pfxForeachCallback)(HWND hWnd);
  struct EnumCallbackParam
  {
    InputMethodContext* pThis;
    pfxForeachCallback pCallback;
  };
  void forEachFirefoxWindow(pfxForeachCallback callback);
  static BOOL CALLBACK enumWndProc(HWND hwnd, LPARAM lParam);

  bool onMessage(PMSG pMsg);
  void onSendMessage(PCWPSTRUCT pMsg);

private:
  HIMC m_savedIMC; // The saved IMC when disable IME.
  bool m_afterInit; // if init is finished.
  struct {
    DWORD convMode;
    DWORD sentMode;
    BOOL openStatus;
  } m_tempStore;

  // Message hook support
private:
  bool startMessageObserve(void);
  void stopMessageObserve(void);
  static std::set<InputMethodContext*> sm_msgListeners;

  static LRESULT CALLBACK MessageProviderProc(int code, WPARAM wParam, LPARAM lParam);
  static LRESULT CALLBACK SendMessageProc(int code, WPARAM wParam, LPARAM lParam);

  static HHOOK sm_hMsgHook; // Message hook for PostMessage
  static HHOOK sm_hSendHook;  // Message hook for SendMessage
};

#endif // _INPUT_METHOD_CONTEXT_H_

#include <windows.h>
#include <assert.h>
#include "InputMethodContext.h"
#include "NativeCodeLogger.h"
#include "Win32Error.h"

#pragma comment(lib, "Imm32")

HMODULE  GetMyModuleHandle(void);
std::set<InputMethodContext*> InputMethodContext::sm_msgListeners;
HHOOK InputMethodContext::sm_hMsgHook = NULL;
HHOOK InputMethodContext::sm_hSendHook = NULL;

InputMethodContext::InputMethodContext(void)
: m_savedIMC(NULL)
, m_afterInit(false)
{
  m_tempStore.convMode = 0;
  m_tempStore.sentMode = 0;
  m_tempStore.openStatus = FALSE;
}

InputMethodContext::~InputMethodContext(void)
{
  //
  // TODO:
  // Should clean up the cached HIMC.
  // Or there will be memory leaks.
  // But because this is a singleton class, we do not care it now.
  //

  stopMessageObserve();
}

bool InputMethodContext::init(void)
{
  startMessageObserve();

  m_afterInit = true;
  return true;
}

bool InputMethodContext::startMessageObserve(void)
{
  if ( sm_msgListeners.empty() )
  {
    assert ( sm_hMsgHook == NULL );
    sm_hMsgHook = ::SetWindowsHookEx(WH_GETMESSAGE, MessageProviderProc, GetMyModuleHandle(), ::GetCurrentThreadId());
    if ( sm_hMsgHook == NULL )
    {
      Win32Error err;
      LOGGER(LOG_ERROR) << "Failed to create post message observable provider (" << err.getErrorCode() << "):" << err.getString() <<endlog;
      return false;
    }
    LOGGER(LOG_INFO) << "PostMessage observable provider " << sm_hMsgHook << " created." << endlog;

    sm_hSendHook = ::SetWindowsHookEx(WH_CALLWNDPROC, SendMessageProc, GetMyModuleHandle(), ::GetCurrentThreadId());
    if ( sm_hSendHook == NULL )
    {
      Win32Error err;
      LOGGER(LOG_ERROR) << "Failed to create get message observable provider (" << err.getErrorCode() << "):" << err.getString() <<endlog;
      UnhookWindowsHookEx(sm_hMsgHook);
      sm_hMsgHook = NULL;
      return false;
    }
    LOGGER(LOG_INFO) << "GetMessage observable provider " << sm_hSendHook << " created." << endlog;
  }

  if ( sm_msgListeners.insert(this).second )
  {
    LOGGER(LOG_INFO) << "Message observer " << this << " added successfully." << endlog;
  }
  else
  {
    LOGGER(LOG_INFO) << "Message observer " << this << " already added, ignore." << endlog;
  }

  return true;
}

void InputMethodContext::stopMessageObserve(void)
{
  if ( sm_hMsgHook == NULL )
    return;

  std::set<InputMethodContext*>::iterator findIt = sm_msgListeners.find(this);
  if ( findIt != sm_msgListeners.end() )
  {
    sm_msgListeners.erase(findIt);

    LOGGER(LOG_INFO) << "Message observer " << this << " removed." << endlog;

    if ( sm_msgListeners.empty() )
    {
      ::UnhookWindowsHookEx(sm_hMsgHook);
      LOGGER(LOG_INFO) << "Observable post message provider " << sm_hMsgHook << " destroyed." << endlog;
      sm_hMsgHook = NULL;
      ::UnhookWindowsHookEx(sm_hSendHook);
      LOGGER(LOG_INFO) << "Observable send message provider " << sm_hSendHook << " destroyed." << endlog;
      sm_hSendHook = NULL;
    }
  }
}

void InputMethodContext::onMessage(PMSG pMsg)
{
  (pMsg);
#if 0
  if ( pMsg->message == WM_INPUTLANGCHANGE 
     || pMsg->message == WM_INPUTLANGCHANGEREQUEST )
  {
    LOGGER(LOG_DEBUG) << "Input language changed by command "
      << (pMsg->message==WM_INPUTLANGCHANGE?"WM_INPUTLANGCHANGE":"WM_INPUTLANGCHANGREQUEST") << endlog;
    if ( m_hwndIME && ::IsWindow(m_hwndIME) )
    {
      if ( m_savedIMC )
      {
        LOGGER(LOG_DEBUG) << "The saved disabled IMC " << m_savedIMC << " is destroyed." << endlog;
        HIMC hTmpImc = m_savedIMC;
        m_savedIMC = NULL;
        ::ImmReleaseContext(m_hwndIME, hTmpImc);

        //
        // Because when we have chance to process this message, the thread have
        // returned from the "set input method to new tab" flow. (Firefox UI is
        // single threaded and WM_INPUTLANGCHANGE/WM_INPUTLANGCHANGEREQUEST is POSTED to message queue.)
        // So if new input method is also disable, it will set m_enable to false and keep the
        // save IMC there, to be changed by this notification.
        //
        if ( ! m_enabled )
          disable();
      }
    }
  }
#endif
}

void InputMethodContext::onSendMessage(PCWPSTRUCT pMsg)
{
  if ( pMsg->message == WM_INPUTLANGCHANGE )
  {
    LOGGER(LOG_DEBUG) << "Input language changed to " << reinterpret_cast<void*>(pMsg->lParam) << endlog;
  }
}

LRESULT CALLBACK InputMethodContext::MessageProviderProc(int code, WPARAM wParam, LPARAM lParam)
{
  if ( HC_ACTION == code )
  {
    UNREFERENCED_PARAMETER(wParam);
    PMSG pMsg = reinterpret_cast<PMSG>(lParam);
    if ( pMsg != NULL )
    {
      for ( std::set<InputMethodContext*>::iterator traIt = sm_msgListeners.begin();
        traIt != sm_msgListeners.end(); ++traIt )
      {
        InputMethodContext* imc = *traIt;
        assert ( imc != NULL );
        imc->onMessage(pMsg);
      }
    }
  }

  return ::CallNextHookEx(0, code, wParam, lParam);
}

LRESULT CALLBACK InputMethodContext::SendMessageProc(int code, WPARAM wParam, LPARAM lParam)
{
  if ( HC_ACTION == code )
  {
    UNREFERENCED_PARAMETER(wParam);
    PCWPSTRUCT pMsg = reinterpret_cast<PCWPSTRUCT>(lParam);
    if ( pMsg != NULL )
    {
      for ( std::set<InputMethodContext*>::iterator traIt = sm_msgListeners.begin();
        traIt != sm_msgListeners.end(); ++traIt )
      {
        InputMethodContext* imc = *traIt;
        assert ( imc != NULL );
        imc->onSendMessage(pMsg);
      }
    }
  }

  return ::CallNextHookEx(0, code, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////

void InputMethodContext::enable(void)
{
  if ( isEnabled() )
  {
    LOGGER(LOG_INFO) << "Input method already enable." << endlog;
    return;
  }

  LOGGER(LOG_INFO) << "Enable input method." << endlog;
  forEachFirefoxWindow(&InputMethodContext::enableWindowIME);
  m_savedIMC = NULL;
}

void InputMethodContext::disable(void)
{
  if ( ! isEnabled() )
  {
    LOGGER(LOG_INFO) << "Input method already disable." << endlog;
    return;
  }

  LOGGER(LOG_INFO) << "Disable input method." << endlog;
  m_savedIMC = NULL;
  forEachFirefoxWindow(&InputMethodContext::disableWindowIME);
}

bool InputMethodContext::hasSavedIMC(void)
{
  return (m_savedIMC!=NULL);
}

bool InputMethodContext::isEnabled(void)
{
  return (m_afterInit && !hasSavedIMC());
}

bool InputMethodContext::enableWindowIME(HWND hwnd)
{
  if ( m_savedIMC != NULL && ::IsWindow(hwnd) )
  {
    ::ImmAssociateContext(hwnd, m_savedIMC);
    //if ( ::IsWindowVisible(hwnd) )
      ::SetFocus(hwnd);
  }

  return true;
}

bool InputMethodContext::disableWindowIME(HWND hwnd)
{
  HIMC hImc = ::ImmGetContext(hwnd);

  // associate context to NULL to disable the IME.
  if ( hImc != NULL )
    ::ImmAssociateContext(hwnd, NULL);

  ::ImmReleaseContext(hwnd, hImc);
  //if ( ::GetParent(hwnd) == NULL )
    ::SetFocus(hwnd);

  m_savedIMC = hImc;

  return false;
}

bool InputMethodContext::setWindowIMEMode(HWND hwnd)
{
  HIMC hImc = ::ImmGetContext(hwnd);
  if ( hImc )
  {
    ::ImmSetConversionStatus(hImc, m_tempStore.convMode, m_tempStore.sentMode);
    LanguageID language = getLanguage();
    if ( language == JAPANESE )
    {
      BOOL openStatus = (m_tempStore.convMode & IME_CMODE_NATIVE)?TRUE:FALSE;
      ::ImmSetOpenStatus(hImc, openStatus);
    }
    ::ImmReleaseContext(hwnd, hImc);
  }

  return true;  // Continue enumerate next window.
}

bool InputMethodContext::setWindowIMEOpenStatus(HWND hwnd)
{
  HIMC hImc = ::ImmGetContext(hwnd);
  if ( hImc )
  {
    ::ImmSetOpenStatus(hImc, m_tempStore.openStatus);
    ::ImmReleaseContext(hwnd, hImc);
  }

  return true;  // Continue to enumerate next window.
}

bool InputMethodContext::getWindowIMEMode(HWND hwnd)
{
  HIMC hImc = ::ImmGetContext(hwnd);
  if ( hImc )
  {
    if ( ! ::ImmGetConversionStatus(hImc, &m_tempStore.convMode, &m_tempStore.sentMode) )
    {
      ::ImmReleaseContext(hwnd, hImc);
      return true;    // try next window to get.
    }

    LanguageID language = getLanguage();
    if ( language == JAPANESE )
    {
      if ( ::ImmGetOpenStatus(hImc) )
        m_tempStore.convMode |= IME_CMODE_NATIVE;
      else
        m_tempStore.convMode &= ~IME_CMODE_NATIVE;
    }

    ::ImmReleaseContext(hwnd, hImc);
    
    return false;   //  already got the value, will not continue search.
  }

  return true;  // try next window to get.
}


bool InputMethodContext::getWindowIMEOpenStatus(HWND hwnd)
{
  HIMC hImc = ::ImmGetContext(hwnd);
  if ( hImc )
  {
    m_tempStore.openStatus = ::ImmGetOpenStatus(hImc);
    ::ImmReleaseContext(hwnd, hImc);
    return false;     // already found, will not continue.
  }

  return true;
}

void InputMethodContext::forEachFirefoxWindow(pfxForeachCallback callback)
{
  EnumCallbackParam params;
  params.pThis = this;
  params.pCallback = callback;
  ::EnumThreadWindows(::GetCurrentThreadId(), &enumWndProc, reinterpret_cast<LPARAM>(&params));
}

BOOL CALLBACK InputMethodContext::enumWndProc(HWND hwnd, LPARAM lParam)
{
  EnumCallbackParam* pParam = reinterpret_cast<EnumCallbackParam*>(lParam);
  assert ( pParam != NULL );
  assert ( pParam->pThis != NULL );
  assert ( pParam->pCallback != NULL );

  InputMethodContext* pThis = pParam->pThis;
  pfxForeachCallback pfxCallback = pParam->pCallback;
  return ((pThis->*pfxCallback)(hwnd)) ? TRUE : FALSE;
}

InputMethodContext::LanguageID InputMethodContext::getLanguage(void)
{
  HKL hKeyboardLayout = ::GetKeyboardLayout(0);
  switch (LOWORD(hKeyboardLayout))
  {
  case LID_TRADITIONAL_CHINESE:
    return TRADITIONAL_CHINESE;
  case LID_JAPANESE:
    return JAPANESE;
  case LID_KOREAN:
    return KOREAN;
  case LID_SIMPLIFIED_CHINESE:
    return SIMPLIFIED_CHINESE;
  default:
    return DEFAULT;
  }
}

#ifndef _USE_IME_FLAGS_FIXUP

void InputMethodContext::getIMEMode(DWORD& convMode, DWORD& sentMode)
{
  forEachFirefoxWindow(&InputMethodContext::getWindowIMEMode);
  convMode = m_tempStore.convMode;
  sentMode = m_tempStore.sentMode;

#if 0
  HWND hWnd = ::GetActiveWindow();
  if ( hWnd )
  {
    HIMC hImc = ::ImmGetContext(hWnd);
    if ( hImc )
    {
      if ( ! ::ImmGetConversionStatus(hImc, &convMode, &sentMode) )
      {
        LanguageID language = getLanguage();
        if ( language == JAPANESE )
        {
          if ( ::ImmGetOpenStatus(hImc) )
            convMode |= IME_CMODE_NATIVE;
          else
            convMode &= ~IME_CMODE_NATIVE;
        }

        ::ImmReleaseContext(hWnd, hImc);
        throw std::runtime_error("Unable to get IMC mode");
      }

      ::ImmReleaseContext(hWnd, hImc);
    }
  }
#endif
}

void InputMethodContext::setIMEMode(DWORD convMode, DWORD sentMode)
{
  m_tempStore.convMode = convMode;
  m_tempStore.sentMode = sentMode;

  forEachFirefoxWindow(&InputMethodContext::setWindowIMEMode);

  LanguageID language = getLanguage();
  if ( language == JAPANESE )
  {
    setOpenStatus(convMode & IME_CMODE_NATIVE);
  }
}

BOOL InputMethodContext::getOpenStatus(void)
{
#if 0
  BOOL openStatus = FALSE;
  HWND hWnd = ::GetActiveWindow();
  if ( hWnd )
  {
    HIMC hImc = ::ImmGetContext(hWnd);
    if ( hImc )
    {
      openStatus = ::ImmGetOpenStatus(hImc);
      ::ImmReleaseContext(hWnd, hImc);
    }
  }

  return openStatus;
#endif
  forEachFirefoxWindow(&InputMethodContext::getWindowIMEOpenStatus);
  return m_tempStore.openStatus;
}

void InputMethodContext::setOpenStatus(bool status)
{
  m_tempStore.openStatus = (status?TRUE:FALSE);

  forEachFirefoxWindow(&InputMethodContext::setWindowIMEOpenStatus);
}

#else // _USE_IME_FLAGS_FIXUP

void InputMethodContext::getIMEMode(DWORD& convMode, DWORD& sentMode, BOOL& englishMode, BOOL& isSentModeNone)
{
  HIMC hImc = getIMC();

  if ( hImc == NULL )
  {
    Win32Error err(::GetLastError());
    LOGGER(LOG_ERROR) << "Unable to get the current IMC: " << err.getErrorCode() << ": " << err.getString() << endlog;
    throw std::runtime_error("Failed to get current IMC");
  }

  ::ImmGetConversionStatus(hImc, &convMode, &sentMode);

  LanguageID language = getLanguage();
  if ( JAPANESE == language || SIMPLIFIED_CHINESE == language )
  {
    if ( ::ImmGetOpenStatus(hImc) )
      convMode |= IME_CMODE_NATIVE;
    else
      convMode &= ~IME_CMODE_NATIVE;
  }

  fixupGotIMEMode(convMode, sentMode, englishMode, isSentModeNone);
}

void InputMethodContext::setIMEMode(DWORD convMode, DWORD sentMode, BOOL englishMode, BOOL isSentModeNone)
{
  fixupSetIMEMode(convMode, sentMode, englishMode, isSentModeNone);

  HIMC hImc = getIMC();
  ::ImmSetConversionStatus(hImc, convMode, sentMode);

  LanguageID language = getLanguage();
  if ( JAPANESE == language )
    ::ImmSetOpenStatus(hImc, !englishMode);
}

//
// Conversion mode availability effective table
// See Microsoft IME sample.
//
static const size_t CONV_FLAG_COUNT = 12;

// Flag for effective options in conversion mode 
static const BOOL fConvMode[5][12] = {	
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Default
  {1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0}, // zh-TW
  {1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1}, // ja
  {1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, // KR
  {1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0}};// zh-CN

// bit field for conversion mode, see Imm.h
// IME_CMODE_ALPHANUMERIC is 0 (default, not bit set, should take care)
// IME_CMODE_LANGUAGE = IME_CMODE_NATIVE | IME_CMODE_KATAKANA, so does not list here.
static const DWORD dwConvModeMsg[12] = {
  IME_CMODE_ALPHANUMERIC,		IME_CMODE_NATIVE,		IME_CMODE_KATAKANA, 
  IME_CMODE_FULLSHAPE,	IME_CMODE_ROMAN, 
  IME_CMODE_CHARCODE,			IME_CMODE_HANJACONVERT, IME_CMODE_SOFTKBD, 
  IME_CMODE_NOCONVERSION,		IME_CMODE_EUDC,			IME_CMODE_SYMBOL, 
  IME_CMODE_FIXED};

static const DWORD IDX_CMODE_NATIVE = 1;

//
// Sentence mode availability effective table
// See Microsoft IME sample.
//
static const size_t SENT_FLAG_COUNT = 6;

// Flag for effective options in sentence mode 
BOOL fSentMode[5][6] = {	
  {0, 0, 0, 0, 0, 0},   // default
  {0, 1, 0, 0, 0, 0},   // zh-TW
  {1, 1, 1, 1, 1, 1},   // ja
  {0, 0, 0, 0, 0, 0},   // kr
  {0, 0, 0, 0, 0, 0}};  // zh-CN

// bit field for sentence mode
DWORD dwSentModeMsg[6] = {
  IME_SMODE_NONE,			IME_SMODE_PLAURALCLAUSE,	IME_SMODE_SINGLECONVERT,	
  IME_SMODE_AUTOMATIC,	IME_SMODE_PHRASEPREDICT,	IME_SMODE_CONVERSATION };

static const DWORD IDX_SMODE_NONE = 0;

void InputMethodContext::fixupGotIMEMode(DWORD& convMode, DWORD& sentMode, BOOL& englishMode, BOOL& isSentModeNone)
{
  DWORD newConvMode = convMode, newSentMode=sentMode;
  LanguageID language = getLanguage();

  // Fixup conversion mode
  if (convMode & dwConvModeMsg[IDX_CMODE_NATIVE] )
    englishMode = FALSE;
  else
    englishMode = TRUE;

  for (size_t i=1; i<CONV_FLAG_COUNT; i++)
  {
    if ( ! fConvMode[language][i] )
    {
      // The flag is not supported, should clear the flag.
      newConvMode &= ~dwConvModeMsg[i];
    }
  }

  // Fixup sentence mode options
  // Set IME_SMODE_NONE option first. 
  if ( ! fSentMode[language][0] || sentMode == IME_SMODE_NONE)
    isSentModeNone = TRUE;

  // Set other sentence option. If IME_SMODE_NONE is active, other options are ignored
  for ( size_t i=1; i<SENT_FLAG_COUNT; ++i )
  {
    if ( ! (fSentMode[language][i]) )
      newSentMode &= ~dwSentModeMsg[i];
  }

  convMode = newConvMode;
  sentMode = newSentMode;
}

void InputMethodContext::fixupSetIMEMode(DWORD& convMode, DWORD& sentMode, BOOL englishMode, BOOL isSentModeNone)
{
  DWORD newConvMode = convMode, newSentMode = sentMode;
  LanguageID language = getLanguage();

  // Fix up set conversion mode options
  // IME_CMODE_ALPHANUMERIC and IME_CMODE_NATIVE are toggle options
  if ( fConvMode[language][0] )
  {
    if ( englishMode )
      newConvMode &= ~IME_CMODE_NATIVE;
    else
      newConvMode |= IME_CMODE_NATIVE;
  }

  for (size_t i=1; i<CONV_FLAG_COUNT; ++i)
  {
    if ( ! fConvMode[language][i] )
    {
      newConvMode &= ~dwSentModeMsg[i];
    }
  }

  // Fix up set sentence mode options
  if ( fSentMode[language][0] )
  {
    if ( isSentModeNone )
      newSentMode |= IME_SMODE_NONE;
    else
      newSentMode &= ~IME_SMODE_NONE;
  }

  for (size_t i=1; i<SENT_FLAG_COUNT; ++i)
  {
    if ( ! fSentMode[language][i] )
    {
      newSentMode &= ~dwSentModeMsg[i];
    }
  }

  convMode = newConvMode;
  sentMode = newSentMode;
}

#endif // _USE_IME_FLAGS_FIXUP

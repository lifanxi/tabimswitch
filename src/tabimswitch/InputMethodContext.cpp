#include <windows.h>
#include <assert.h>
#include "InputMethodContext.h"
#include "NativeCodeLogger.h"
#include "Win32Error.h"

#pragma comment(lib, "Imm32")

InputMethodContext::InputMethodContext(void)
: m_savedIMC(NULL)
, m_hwndIME(NULL)
{

}

InputMethodContext::~InputMethodContext(void)
{
  //
  // TODO:
  // Should clean up the cached HIMC.
  // Or there will be memory leaks.
  // But because this is a singleton class, we do not care it now.
  //
}

bool InputMethodContext::init(void)
{
  if ( ! refreshIMC() )
    return false;

  forEachFirefoxWindow(&InputMethodContext::getIMEWindow);
  if ( NULL == m_hwndIME )
  {
    LOGGER(LOG_ERROR) << "Failed to create Application IME window." << endlog;
    return false;
  }
  LOGGER(LOG_DEBUG) << "Application IME window: " << m_hwndIME << endlog;

  return true;
}

bool InputMethodContext::refreshIMC(void)
{
  m_appIMC.clear();

  forEachFirefoxWindow(&InputMethodContext::fillAppIMC);

  if ( m_appIMC.empty() )
  {
    Win32Error err;
    LOGGER(LOG_ERROR) << "Unable to get window IMC: " << err.getErrorCode() << ": " << err.getString() << endlog;
    return false;
  }

  LOGGER(LOG_DEBUG) << "Window IMC count: " << m_appIMC.size() << endlog;

  return true;
}

HIMC InputMethodContext::getIMC(void)
{
  if ( m_savedIMC != NULL )
    return m_savedIMC;

  if ( m_appIMC.empty() )
  {
    if ( ! refreshIMC() )
      return NULL;
  }

  //
  // TRICK:
  // No matter how much IMCs, we just use the first
  // Is it an acceptable method?
  //
  return *m_appIMC.begin();
}

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
    LOGGER(LOG_INFO) << "Input methdo already disable." << endlog;
    return;
  }

  LOGGER(LOG_INFO) << "Disable input method." << endlog;
  m_savedIMC = getIMC();
  forEachFirefoxWindow(&InputMethodContext::disableWindowIME);
}

bool InputMethodContext::isEnabled(void)
{
  return (m_savedIMC == NULL);
}

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

bool InputMethodContext::enableWindowIME(HWND hwnd)
{
  assert ( m_savedIMC != NULL );
  ::ImmAssociateContext(hwnd, m_savedIMC);
  return true;
}

bool InputMethodContext::disableWindowIME(HWND hwnd)
{
  assert ( m_savedIMC == NULL );

  HIMC hImc = ::ImmGetContext(hwnd);

  // associate context to NULL to disable the IME.
  ::ImmAssociateContext(hwnd, NULL);

  if ( hImc != NULL )
  {
    ::ImmReleaseContext(hwnd, hImc);
  }

  return true;
}

bool InputMethodContext::fillAppIMC(HWND hwnd)
{
  if ( hwnd != NULL && ::IsWindow(hwnd) )
  {
    HIMC hImc = ::ImmGetContext(hwnd);
    if ( ! m_appIMC.insert(hImc).second )
    {
      ::ImmReleaseContext(hwnd, hImc);
    }
  }

  return true;  // continue search next.
}

bool InputMethodContext::getIMEWindow(HWND hwnd)
{
  if ( hwnd != NULL && ::IsWindow(hwnd) )
    m_hwndIME = ::ImmGetDefaultIMEWnd(hwnd);

  if ( m_hwndIME != NULL )
    return false;     // created the HWND, will not try more
  else
    return true;      // try next window to create IME HWND
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


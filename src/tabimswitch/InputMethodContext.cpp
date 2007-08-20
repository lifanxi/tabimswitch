#include <windows.h>
#include <assert.h>
#include "InputMethodContext.h"
#include "NativeCodeLogger.h"

#pragma comment(lib, "Imm32")

InputMethodContext& InputMethodContext::get(void)
{
  static InputMethodContext imc;
  return imc;
}

InputMethodContext::InputMethodContext(void)
: m_enable(false)
, m_enabledIMC(NULL)
{

}

bool InputMethodContext::init(void)
{
  if ( m_enabledIMC != NULL )
    return true;

  m_enabledIMC = ::ImmCreateContext();
  if ( ! ::ImmSetOpenStatus(m_enabledIMC, TRUE) )
  {
    LOGGER(LOG_ERROR) << "Init input method context failed: " << ::GetLastError() << endlog;
    return false;
  }

  return true;
}

InputMethodContext::~InputMethodContext(void)
{
  if ( m_enabledIMC != NULL )
  {
    ::ImmDestroyContext(m_enabledIMC);
    m_enabledIMC = NULL;
  }
}

void InputMethodContext::enable(void)
{
  assert ( m_enabledIMC != NULL );
  LOGGER(LOG_INFO) << "Enable input method." << endlog;
  forEachFirefoxWindow(&InputMethodContext::enableWindowIME);
}

void InputMethodContext::disable(void)
{
  LOGGER(LOG_INFO) << "Disable input method." << endlog;
  forEachFirefoxWindow(&InputMethodContext::disableWindowIME);
}

bool InputMethodContext::isEnabled(void)
{
  forEachFirefoxWindow(&InputMethodContext::getWindowIMEStatus);
  return m_enable;
}

bool InputMethodContext::enableWindowIME(HWND hwnd)
{
  if ( ! ::ImmAssociateContext(hwnd, m_enabledIMC) )
  {
    LOGGER(LOG_ERROR) << "Unable to enable IME for window " << hwnd << endlog;
  }

  m_enable = true;

  return true;
}

bool InputMethodContext::disableWindowIME(HWND hwnd)
{
  if ( ! ::ImmAssociateContextEx(hwnd, NULL, IACE_DEFAULT) )
  {
    LOGGER(LOG_ERROR) << "Unable to disable IME for window " << hwnd << endlog;
  }

  m_enable = false;

  return true;
}

bool InputMethodContext::getWindowIMEStatus(HWND hwnd)
{
  HIMC hImc = ::ImmGetContext(hwnd);
  m_enable = ::ImmGetOpenStatus(hImc) ? true : false;
  return false;
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

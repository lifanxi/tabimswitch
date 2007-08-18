# pragma warning(disable:4512)  // assignment operator could not be generated
# pragma warning(disable:4127)  // conditional expression is constant

#include "TabImSwitchApp.h"
#include <nsStringApi.h>
#include <Windows.h>

#include "NativeCodeLogger.h"

/* Implementation file */
NS_IMPL_ISUPPORTS1(CTabImSwitchApp, ITabImSwitchApp)

CTabImSwitchApp::CTabImSwitchApp()
{
  LOGGER(LOG_DEBUG) << "New instance of TabImSwitchApp is created: " << this << endlog;
}

CTabImSwitchApp::~CTabImSwitchApp()
{
  LOGGER(LOG_DEBUG) << "TabImSwitchApp destroyed: " << this << endlog;
}

/* attribute AString currentKeyboardLayoutName; */
NS_IMETHODIMP CTabImSwitchApp::GetCurrentKeyboardLayoutName(nsAString & aCurrentKeyboardLayoutName)
{
  HKL curKL = ::GetKeyboardLayout(::GetCurrentThreadId());
  wchar_t strCurKL[16];
  swprintf_s(strCurKL, L"%08X", curKL);
  aCurrentKeyboardLayoutName = strCurKL;
  LOGGER(LOG_DEBUG) << "Current keyboard layout name: " << strCurKL << endlog;

  return NS_OK;
}

NS_IMETHODIMP CTabImSwitchApp::SetCurrentKeyboardLayoutName(const nsAString & aCurrentKeyboardLayoutName)
{
  if ( aCurrentKeyboardLayoutName.IsEmpty() )
    return NS_ERROR_ILLEGAL_VALUE;

  const nsAString::char_type* pKblName = aCurrentKeyboardLayoutName.BeginReading();
  if ( pKblName == NULL )
    return NS_ERROR_UNEXPECTED;

  HKL theKL = 0;
  swscanf_s(pKblName, L"%x", &theKL);

  LOGGER(LOG_DEBUG) << "Set current keyboard layout HKL to " << theKL << endlog;

  HKL hOldKL = ::ActivateKeyboardLayout(theKL, KLF_SETFORPROCESS);
  if ( hOldKL != NULL )
  {
    return NS_OK;
  }

  return NS_ERROR_UNEXPECTED;
}

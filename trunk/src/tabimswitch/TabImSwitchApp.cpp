# pragma warning(disable:4512)  // assignment operator could not be generated
# pragma warning(disable:4127)  // conditional expression is constant

#include "TabImSwitchApp.h"
#include <nsStringApi.h>
#include <Windows.h>

/* Implementation file */
NS_IMPL_ISUPPORTS1(CTabImSwitchApp, ITabImSwitchApp)

CTabImSwitchApp::CTabImSwitchApp()
{
  /* member initializers and constructor code */
}

CTabImSwitchApp::~CTabImSwitchApp()
{
  /* destructor code */
}

/* attribute AString currentKeyboardLayoutName; */
NS_IMETHODIMP CTabImSwitchApp::GetCurrentKeyboardLayoutName(nsAString & aCurrentKeyboardLayoutName)
{
  nsAString::char_type kbdLayoutName[KL_NAMELENGTH];
  if ( ::GetKeyboardLayoutNameW(static_cast<LPWSTR>(kbdLayoutName)) )
  {
    aCurrentKeyboardLayoutName = kbdLayoutName;
  }

  return NS_OK;
}

NS_IMETHODIMP CTabImSwitchApp::SetCurrentKeyboardLayoutName(const nsAString & aCurrentKeyboardLayoutName)
{
  if ( ! aCurrentKeyboardLayoutName.IsEmpty() )
  {
    const nsAString::char_type* pKblName = aCurrentKeyboardLayoutName.BeginReading();
    if ( pKblName == NULL )
      return NS_ERROR_UNEXPECTED;

    HKL hKeyboadLayout = ::LoadKeyboardLayoutW(static_cast<LPCWSTR>(pKblName),
        /*KLF_ACTIVATE|KLF_SETFORPROCESS|*/KLF_REPLACELANG);
    if ( hKeyboadLayout != NULL )
    {
      HKL hOldKeyboard = ::ActivateKeyboardLayout(hKeyboadLayout, KLF_SETFORPROCESS);
      if ( hOldKeyboard != NULL )
      {
        return NS_OK;
      }
    }

    return NS_ERROR_UNEXPECTED;
  }

  return NS_ERROR_ILLEGAL_VALUE;
}

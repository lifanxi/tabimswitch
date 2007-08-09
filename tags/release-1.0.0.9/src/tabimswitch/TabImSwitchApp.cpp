# pragma warning(disable:4512)  // assignment operator could not be generated
# pragma warning(disable:4127)  // conditional expression is constant

#include "TabImSwitchApp.h"
#include <nsStringApi.h>
#include <Windows.h>
#include <Shlwapi.h>

#pragma comment(lib, "shlwapi")

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

    LPCWSTR pszKblName = static_cast<LPCWSTR>(pKblName);
    WCHAR primaryKblName[KL_NAMELENGTH];

    const size_t DEVICE_ID_LEN = 5;
    WCHAR deviceId[DEVICE_ID_LEN] = L"0000";
    lstrcpynW(deviceId, pKblName, _countof(deviceId));
    if ( lstrcmpW(deviceId, L"0000") == 0 )
    {
      const size_t localeIdOffset = 4;
      wnsprintfW(primaryKblName, _countof(primaryKblName), L"%s%s", pKblName+localeIdOffset, pKblName+localeIdOffset);
      pszKblName = primaryKblName;
    }

    HKL hKeyboadLayout = ::LoadKeyboardLayoutW(pszKblName, 0);
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

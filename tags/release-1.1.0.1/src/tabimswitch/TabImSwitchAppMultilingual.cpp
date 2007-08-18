# pragma warning(disable:4512)  // assignment operator could not be generated
# pragma warning(disable:4127)  // conditional expression is constant

#include "TabImSwitchAppMultilingual.h"
#include <nsStringApi.h>
#include <Windows.h>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi")

#include "NativeCodeLogger.h"

NS_IMPL_ISUPPORTS1(CTabImSwitchAppMultilingual, ITabImSwitchApp)

CTabImSwitchAppMultilingual::CTabImSwitchAppMultilingual()
{
  LOGGER(LOG_DEBUG) << "New instance of TabImSwitchAppMultilingual is created: " << this << endlog;
}

CTabImSwitchAppMultilingual::~CTabImSwitchAppMultilingual()
{
  LOGGER(LOG_DEBUG) << "TabImSwitchAppMultilingual destroyed: " << this << endlog;
}

/* attribute AString currentKeyboardLayoutName; */
NS_IMETHODIMP CTabImSwitchAppMultilingual::GetCurrentKeyboardLayoutName(nsAString & aCurrentKeyboardLayoutName)
{
  nsAString::char_type kbdLayoutName[KL_NAMELENGTH];
  if ( ::GetKeyboardLayoutNameW(static_cast<LPWSTR>(kbdLayoutName)) )
  {
    LOGGER(LOG_DEBUG) << "Current keyboard layout name: " << kbdLayoutName << endlog;
    aCurrentKeyboardLayoutName = kbdLayoutName;
  }

  return NS_OK;
}

NS_IMETHODIMP CTabImSwitchAppMultilingual::SetCurrentKeyboardLayoutName(const nsAString & aCurrentKeyboardLayoutName)
{
  if ( aCurrentKeyboardLayoutName.IsEmpty() )
    return NS_ERROR_ILLEGAL_VALUE;

  const nsAString::char_type* pKblName = aCurrentKeyboardLayoutName.BeginReading();
  if ( pKblName == NULL )
    return NS_ERROR_UNEXPECTED;

  LPCWSTR pszKblName = static_cast<LPCWSTR>(pKblName);

  WCHAR primaryKblName[KL_NAMELENGTH];
  if ( wcsncmp(pszKblName, L"0000", 4) == 0 )
  {
    wnsprintfW(primaryKblName, _countof(primaryKblName), L"%s%s", pszKblName+4, pszKblName+4);
    pszKblName = primaryKblName;
  }

  HKL hKeyboadLayout = ::LoadKeyboardLayoutW(pszKblName, 0);
  if ( hKeyboadLayout != NULL )
  {
    HKL hOldKeyboard = ::ActivateKeyboardLayout(hKeyboadLayout, KLF_SETFORPROCESS);
    if ( hOldKeyboard != NULL )
    {
      LOGGER(LOG_DEBUG) << "Set current keyboard layout to " << pszKblName;
      return NS_OK;
    }
  }

  return NS_ERROR_UNEXPECTED;
}

#include <string>
#include <windows.h>
#include <assert.h>
#include <string.h>
#include <wchar.h>

#include <Shlwapi.h>
#pragma comment(lib, "shlwapi")

#include "NativeCodeLogger.h"
#include "KeyboardLayout.h"

std::wstring DefaultKeyboardLayout::getCurrent(void)
{
  HKL curKL = ::GetKeyboardLayout(::GetCurrentThreadId());
  wchar_t strCurKL[16];
  swprintf_s(strCurKL, L"%08X", curKL);
  LOGGER(LOG_DEBUG) << "Current keyboard layout name: " << strCurKL << endlog;
  return std::wstring(strCurKL);
}

bool DefaultKeyboardLayout::setCurrent(std::wstring const& kbl)
{
  assert (!kbl.empty());

  HKL theKL = 0;
  swscanf_s(kbl.c_str(), L"%x", &theKL);

  LOGGER(LOG_DEBUG) << "Set current keyboard layout HKL to " << theKL << endlog;

  HKL hOldKL = ::ActivateKeyboardLayout(theKL, KLF_SETFORPROCESS);
  return ( hOldKL != NULL );
}

char const* DefaultKeyboardLayout::getName(void)
{
  return "Non-Multilingual keyboard layout environment.";
}

std::wstring MultilingualKeyboardLayout::getCurrent(void)
{
  wchar_t kbdLayoutName[KL_NAMELENGTH];
  if ( ::GetKeyboardLayoutNameW(kbdLayoutName) )
  {
    LOGGER(LOG_DEBUG) << "Current keyboard layout name: " << kbdLayoutName << endlog;
    return std::wstring(kbdLayoutName);
  }

  throw std::runtime_error("Unable to get current keyboard layout name.");
}

bool MultilingualKeyboardLayout::setCurrent(std::wstring const& kbl)
{
  LPCWSTR pszKblName = kbl.c_str();

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
      return true;
    }
  }

  return false;
}

char const* MultilingualKeyboardLayout::getName(void)
{
  return "Multilingual keyboard layout environment.";
}


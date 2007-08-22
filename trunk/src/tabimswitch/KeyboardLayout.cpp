#include <string>
#include <set>
#include <windows.h>
#include <assert.h>
#include <string.h>
#include <wchar.h>

#include <Shlwapi.h>
#pragma comment(lib, "shlwapi")

#include "NativeCodeLogger.h"
#include "KeyboardLayout.h"

HMODULE GetMyModuleHandle(void);

//////////////////////////////////////////////////////////////////////////
// Class Define
//
class DefaultKeyboardLayout
  : public KeyboardLayout
{
public:
  DefaultKeyboardLayout(void);
  ~DefaultKeyboardLayout(void);

protected:
  bool init(void);
  std::wstring getCurrent(void);
  void setCurrent(std::wstring const& kbl);
  bool isMultilingual(void) const;

  HHOOK m_hMsgHook;
  HKL m_curKL;
};

class MultilingualKeyboardLayout
  : public KeyboardLayout
{
protected:
  bool init(void);
  std::wstring getCurrent(void);
  void setCurrent(std::wstring const& kbl);
  bool isMultilingual(void) const;
};

//////////////////////////////////////////////////////////////////////////
// Base class implementation
//
KeyboardLayout* KeyboardLayout::createDefault(void)
{
  return new DefaultKeyboardLayout();
}

KeyboardLayout* KeyboardLayout::createMultilingual(void)
{
  return new MultilingualKeyboardLayout();
}

KeyboardLayout* KeyboardLayout::createByEnvironment(void)
{
  if ( isMultilingualEnv() )
    return createMultilingual();
  else
    return createDefault();
}

bool KeyboardLayout::isMultilingualEnv(void)
{
  HKL sysHKLs[64] = {0};
  int count = ::GetKeyboardLayoutList(_countof(sysHKLs), sysHKLs);
  std::set<WORD> sysLangs;
  for ( int i=0; i<count; ++i )
  {
    LOGGER(LOG_DEBUG) << "System keyboard layout: " << sysHKLs[i] << endlog;
    WORD langID = LOWORD(sysHKLs[i]);
    if ( sysLangs.find(langID) != sysLangs.end() )
      continue;

    sysLangs.insert(langID);
  }

  size_t langCount = sysLangs.size();
  LOGGER(LOG_DEBUG) << "System language count: " << langCount << endlog;

  //
  // To fix the status of old TabIMSwitch users.
  // Because old version of TabIMSwitch users (<=1.0.0.9) will add an English input method
  // automatically. So if we found that the system has English language (0409) and the
  // language count is 2, we think that's not multilingual environment.
  //
  static const WORD LID_ENGLISH = 0x0409;
  if ( sysLangs.find(LID_ENGLISH) != sysLangs.end() && langCount == 2 )
  {
    langCount = 1;
  }

  LOGGER(LOG_DEBUG) << "Multilingual environment detection: " << (langCount==1?"no":"yes") << endlog;

  return (langCount!=1);
}

//////////////////////////////////////////////////////////////////////////
// DefaultKeyboardLayout
//
DefaultKeyboardLayout::DefaultKeyboardLayout(void)
: m_curKL(::GetKeyboardLayout(::GetCurrentThreadId()))
, m_hMsgHook(NULL)
{
}

DefaultKeyboardLayout::~DefaultKeyboardLayout(void)
{
  if ( m_hMsgHook )
  {
    ::UnhookWindowsHookEx(m_hMsgHook);
    m_hMsgHook = NULL;
  }
}


bool DefaultKeyboardLayout::init(void)
{
  if ( m_hMsgHook != NULL )
    return true;
#if 0
  LOGGER(DEBUG) << "Set up message hook for  thread " << ::GetCurrentThreadId() << endl;
  m_hMsgHook = ::SetWindowsHookEx(WH_GETMESSAGE, &GetMsgProc, GetMyModuleHandle(), ::GetCurrentThreadId());
#endif 
  return true;
}

std::wstring DefaultKeyboardLayout::getCurrent(void)
{
  HKL curKL = ::GetKeyboardLayout(0);
  wchar_t strCurKL[16];
  swprintf_s(strCurKL, L"%08X", curKL);
  LOGGER(LOG_TRACE) << "Current keyboard layout name: " << strCurKL << endlog;
  return std::wstring(strCurKL);
}

void DefaultKeyboardLayout::setCurrent(std::wstring const& kbl)
{
  assert (!kbl.empty());

  HKL theKL = 0;
  if ( 1 != swscanf_s(kbl.c_str(), L"%x", &theKL) )
    throw std::runtime_error("Invalid format of keyboard layout.");

  LOGGER(LOG_TRACE) << "Set current keyboard layout HKL to " << theKL << endlog;

  HKL hOldKL = ::ActivateKeyboardLayout(theKL, KLF_SETFORPROCESS);
  if ( NULL == hOldKL )
    throw std::runtime_error("Unable to activate keyboard layout.");
}

bool DefaultKeyboardLayout::isMultilingual(void) const { return false; }

//////////////////////////////////////////////////////////////////////////
// Multilingual keyboard layout
//
bool MultilingualKeyboardLayout::init(void)
{
  return true;
}

std::wstring MultilingualKeyboardLayout::getCurrent(void)
{
  wchar_t kbdLayoutName[KL_NAMELENGTH];
  if ( ::GetKeyboardLayoutNameW(kbdLayoutName) )
  {
    LOGGER(LOG_TRACE) << "Current keyboard layout name: " << kbdLayoutName << endlog;
    return std::wstring(kbdLayoutName);
  }

  throw std::runtime_error("Unable to get current keyboard layout name.");
}

void MultilingualKeyboardLayout::setCurrent(std::wstring const& kbl)
{
  LPCWSTR pszKblName = kbl.c_str();

  WCHAR primaryKblName[KL_NAMELENGTH];
  if ( wcsncmp(pszKblName, L"0000", 4) == 0 )
  {
    wnsprintfW(primaryKblName, _countof(primaryKblName), L"%s%s", pszKblName+4, pszKblName+4);
    pszKblName = primaryKblName;
  }

  HKL hKeyboadLayout = ::LoadKeyboardLayoutW(pszKblName, 0);
  if ( hKeyboadLayout == NULL )
    throw std::runtime_error("Unable to load keyboard layout.");

  HKL hOldKeyboard = ::ActivateKeyboardLayout(hKeyboadLayout, KLF_SETFORPROCESS);
  if ( hOldKeyboard == NULL )
    throw std::runtime_error("Unable to activate keyboard layout.");

  LOGGER(LOG_TRACE) << "Set current keyboard layout to " << pszKblName;
}

bool MultilingualKeyboardLayout::isMultilingual(void) const { return true; }

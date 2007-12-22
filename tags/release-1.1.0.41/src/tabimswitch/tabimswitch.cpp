
#include <windows.h>

#pragma warning(disable:4127)   // conditional expression is constant
#pragma warning(disable:4100)   // unreferenced formal parameter

#include <nsIGenericFactory.h>

#include "TabImSwitchApp.h"
#include "DebugLogger.h"
#include "InputMethod.h"

NS_GENERIC_FACTORY_CONSTRUCTOR(CTabImSwitchApp)
NS_GENERIC_FACTORY_CONSTRUCTOR(CDebugLogger)
NS_GENERIC_FACTORY_CONSTRUCTOR(CInputMethod)

static nsModuleComponentInfo components[] =
{
  {
    TABIMSWITCH_APP_CLASSNAME, 
    TABIMSWITCH_APP_CID,
    TABIMSWITCH_APP_CONTRACTID,
    CTabImSwitchAppConstructor
  },

  {
    TABIMSWITCH_DEBUG_LOGGER_CLASSNAME, 
    TABIMSWITCH_DEBUG_LOGGER_CID,
    TABIMSWITCH_DEBUG_LOGGER_CONTRACTID,
    CDebugLoggerConstructor
  },

  {
    INPUTMETHOD_CLASSNAME, 
    INPUTMETHOD_CID,
    INPUTMETHOD_CONTRACTID,
    CInputMethodConstructor
  },
};

NS_IMPL_NSGETMODULE("TabImSwitchModule", components) 

HMODULE g_hModule = NULL;

HMODULE GetMyModuleHandle(void) { return g_hModule; }

BOOL APIENTRY DllMain(HMODULE hMod, DWORD, LPVOID)
{
  g_hModule = hMod;
  return TRUE;
}

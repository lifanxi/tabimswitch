
#include <windows.h>

#pragma warning(disable:4127)   // conditional expression is constant
#pragma warning(disable:4100)   // unreferenced formal parameter

#include <nsIGenericFactory.h>

#include "SystemImManager.h"
#include "TabImSwitchApp.h"
#include "TabImSwitchAppMultilingual.h"
#include "DebugLogger.h"

NS_GENERIC_FACTORY_CONSTRUCTOR(CSystemInputMethodManager)
NS_GENERIC_FACTORY_CONSTRUCTOR(CTabImSwitchApp)
NS_GENERIC_FACTORY_CONSTRUCTOR(CTabImSwitchAppMultilingual)
NS_GENERIC_FACTORY_CONSTRUCTOR(CDebugLogger)

static nsModuleComponentInfo components[] =
{
  {
    TABIMSWITCH_SYSIM_MANAGER_CLASSNAME, 
    TABIMSWITCH_SYSIM_MANAGER_CID,
    TABIMSWITCH_SYSIM_MANAGER_CONTRACTID,
    CSystemInputMethodManagerConstructor
  },

  {
    TABIMSWITCH_APP_CLASSNAME, 
    TABIMSWITCH_APP_CID,
    TABIMSWITCH_APP_CONTRACTID,
    CTabImSwitchAppConstructor
  },

  {
    TABIMSWITCH_APPML_CLASSNAME, 
    TABIMSWITCH_APPML_CID,
    TABIMSWITCH_APPML_CONTRACTID,
    CTabImSwitchAppMultilingualConstructor
  },

  {
    TABIMSWITCH_DEBUG_LOGGER_CLASSNAME, 
    TABIMSWITCH_DEBUG_LOGGER_CID,
    TABIMSWITCH_DEBUG_LOGGER_CONTRACTID,
    CDebugLoggerConstructor
  },
};

NS_IMPL_NSGETMODULE("TabImSwitchModule", components) 

BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID)
{
    return TRUE;
}

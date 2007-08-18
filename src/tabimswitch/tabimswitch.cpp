
#include <windows.h>

#pragma warning(disable:4127)   // conditional expression is constant
#pragma warning(disable:4100)   // unreferenced formal parameter

#include <nsIGenericFactory.h>

#include "TabImSwitchApp.h"
#include "DebugLogger.h"

NS_GENERIC_FACTORY_CONSTRUCTOR(CTabImSwitchApp)
NS_GENERIC_FACTORY_CONSTRUCTOR(CDebugLogger)

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
};

NS_IMPL_NSGETMODULE("TabImSwitchModule", components) 

BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID)
{
    return TRUE;
}

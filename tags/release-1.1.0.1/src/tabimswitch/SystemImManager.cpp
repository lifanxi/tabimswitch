# pragma warning(disable:4512)  // assignment operator could not be generated
# pragma warning(disable:4127)  // conditional expression is constant

#include "SystemImManager.h"
#include "NativeCodeLogger.h"
#include <nsStringApi.h>
#include <windows.h>

#include <sstream>
#include <iomanip>
using std::setw;
using std::setfill;
using std::hex;

NS_IMPL_ISUPPORTS1(CSystemInputMethodManager, ISystemInputMethodManager)

CSystemInputMethodManager::CSystemInputMethodManager()
{
  LOGGER(LOG_DEBUG) << "New System Input method manager created: " << this << endlog;
}

CSystemInputMethodManager::~CSystemInputMethodManager()
{
  LOGGER(LOG_DEBUG) << "System Input method manager destroyed: " << this << endlog;
}

/* readonly attribute AString inputMethodList; */
NS_IMETHODIMP CSystemInputMethodManager::GetInputMethodList(nsAString & aInputMethodList)
{
  HKL sysHKLs[64] = {0};
  int count = ::GetKeyboardLayoutList(_countof(sysHKLs), sysHKLs);

  std::wostringstream oss;
  for ( int i=0; i<count; ++i )
  {
    LOGGER(LOG_DEBUG) << "System loaded input method: " << sysHKLs[i] << endlog;
    oss << hex << setw(8) << setfill(L'0') << sysHKLs[i] << SEPARATOR;
  }

  aInputMethodList = oss.str().c_str();

  return NS_OK;
}

/* readonly attribute char listSeparator; */
NS_IMETHODIMP CSystemInputMethodManager::GetListSeparator(char *aListSeparator)
{
  if ( aListSeparator == NULL )
    return NS_ERROR_NULL_POINTER;

  *aListSeparator = SEPARATOR;

  return NS_OK;
}


#pragma warning(disable:4512)  // assignment operator could not be generated
#pragma warning(disable:4127)  // conditional expression is constant

#include <memory>
#include <iomanip>
#include <new>
#include <Windows.h>
#include "NativeCodeLogger.h"
#include <nsStringApi.h>
#include "tabimswitch.h"
#include "TabImSwitchApp.h"
#include "SystemInputMethod.h"
#include "KeyboardLayout.h"
#include "InputMethodContext.h"
#include "InputMethod.h"

using std::hex;
using std::setw;
using std::setfill;
using std::nothrow;

NS_IMPL_ISUPPORTS1(CTabImSwitchApp, ITabImSwitchApp)

CTabImSwitchApp::CTabImSwitchApp()
: m_sysIME(SystemInputMethod::get())
{
  LOGGER(LOG_DEBUG) << "New instance of TabImSwitchApp with " 
    << (m_sysIME.isMultilingualEnvironment()?"multilingual":"non-multilingual")
    << " keyboard layout manager: " << this << endlog;
}

CTabImSwitchApp::~CTabImSwitchApp()
{
  LOGGER(LOG_DEBUG) << "TabImSwitchApp destroyed: " << this << endlog;
}

/* attribute long multilingualPolicy; */
NS_IMETHODIMP CTabImSwitchApp::GetMultilingualPolicy(PRInt32 *aMultilingualPolicy)
{
  if ( aMultilingualPolicy == NULL )
    return NS_ERROR_NULL_POINTER;

  try
  {
    *aMultilingualPolicy = m_sysIME.getKeyboardLayoutManagerType();
    return NS_OK;
  }
  catch (std::exception& e)
  {
    LOGGER(LOG_ERROR) << "Unable to get multilingual policy: "<< e.what() << endlog;
  }

  return NS_ERROR_UNEXPECTED;
}

NS_IMETHODIMP CTabImSwitchApp::SetMultilingualPolicy(PRInt32 aMultilingualPolicy)
{
  if ( aMultilingualPolicy < SystemInputMethod::AutoKeyboardLayoutManager
    || aMultilingualPolicy > SystemInputMethod::MultilingualKeyboardLayoutManager)
  {
    return NS_ERROR_ILLEGAL_VALUE;
  }

  try
  {
    m_sysIME.useKeyboardLayoutManager(static_cast<SystemInputMethod::KeyboardLayoutManagerType>(aMultilingualPolicy));
    return NS_OK;
  }
  catch (std::exception& e)
  {
    LOGGER(LOG_ERROR) << "Unable to set multilingual policy: "<< e.what() << endlog;
  }
  return NS_ERROR_UNEXPECTED;
}

/* readonly attribute boolean multilingual; */
NS_IMETHODIMP CTabImSwitchApp::GetMultilingual(PRBool *aMultilingual)
{
  if ( aMultilingual == NULL )
    return NS_ERROR_NULL_POINTER;

  try
  {
    *aMultilingual = (m_sysIME.isMultilingualEnvironment()?PR_TRUE:PR_FALSE);
    return NS_OK;
  }
  catch (std::exception& e)
  {
    LOGGER(LOG_ERROR) << "Unable to get multilingual status: "<< e.what() << endlog;
  }

  return NS_ERROR_UNEXPECTED;
}

/* void init (); */
NS_IMETHODIMP CTabImSwitchApp::Init()
{
  try
  {
    m_sysIME.init();
    return NS_OK;
  }
  catch (std::exception& e)
  {
    LOGGER(LOG_ERROR) << "Unable to init TabIMSwitchApp: "<< e.what() << endlog;
  }

  return NS_ERROR_UNEXPECTED;
}

/* readonly attribute AString inputMethodList; */
NS_IMETHODIMP CTabImSwitchApp::GetInputMethodList(nsAString & aInputMethodList)
{
  try
  {
    aInputMethodList = m_sysIME.getInputMethodList().c_str();
    return NS_OK;
  }
  catch (std::exception& e)
  {
    LOGGER(LOG_ERROR) << "Unable to Get Input Method List: "<< e.what() << endlog;
  }

  return NS_ERROR_UNEXPECTED;
}

/* readonly attribute char listSeparator; */
NS_IMETHODIMP CTabImSwitchApp::GetListSeparator(PRUnichar *aListSeparator)
{
  if ( aListSeparator == NULL )
    return NS_ERROR_NULL_POINTER;

  *aListSeparator = m_sysIME.getInputMethodListSeparator();

  return NS_OK;
}

//////////////////////////////////////////////////////////////////////////
// Helper functions.
//


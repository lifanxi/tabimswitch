#include "NativeCodeLogger.h"

template <typename KeyboardLayout>
NS_IMPL_ADDREF(CTabImSwitchAppImpl<KeyboardLayout>)

template <typename KeyboardLayout>
NS_IMPL_RELEASE(CTabImSwitchAppImpl<KeyboardLayout>)

template <typename KeyboardLayout>
NS_IMPL_QUERY_INTERFACE1(CTabImSwitchAppImpl<KeyboardLayout>, ITabImSwitchApp)

template <typename KeyboardLayout>
CTabImSwitchAppImpl<KeyboardLayout>::CTabImSwitchAppImpl()
{
  LOGGER(LOG_DEBUG) << "New instance of TabImSwitchApp with " << KeyboardLayout::getName() << ": " << this << endlog;
}

template <typename KeyboardLayout>
CTabImSwitchAppImpl<KeyboardLayout>::~CTabImSwitchAppImpl()
{
  LOGGER(LOG_DEBUG) << "TabImSwitchApp destroyed: " << this << endlog;
}

/* attribute AString currentKeyboardLayoutName; */
template <typename KeyboardLayout>
NS_IMETHODIMP CTabImSwitchAppImpl<KeyboardLayout>::GetCurrentKeyboardLayoutName(nsAString & aCurrentKeyboardLayoutName)
{
  try
  {
    aCurrentKeyboardLayoutName = KeyboardLayout::getCurrent().c_str();
    aCurrentKeyboardLayoutName.Append((InputMethodContext::get().isEnabled()?L",E":L",D"));
    return NS_OK;
  }
  catch (std::exception& e)
  {
    LOGGER(LOG_ERROR) << "Unable to get current keyboard layout: "<< e.what() << endlog;
  }

  return NS_ERROR_UNEXPECTED;
}

template <typename KeyboardLayout>
NS_IMETHODIMP CTabImSwitchAppImpl<KeyboardLayout>::SetCurrentKeyboardLayoutName(const nsAString & aCurrentKeyboardLayoutName)
{
  if ( aCurrentKeyboardLayoutName.IsEmpty() )
    return NS_ERROR_ILLEGAL_VALUE;

  const nsAString::char_type* pKblName = aCurrentKeyboardLayoutName.BeginReading();
  if ( pKblName == NULL )
    return NS_ERROR_UNEXPECTED;

  std::wstring imString(pKblName);
  size_t idxOfSep = imString.find(L',');
  std::wstring kblName = imString.substr(0, idxOfSep);
  if ( ! KeyboardLayout::setCurrent(kblName) )
    return NS_ERROR_UNEXPECTED;

  if ( imString.length() >= idxOfSep+1 )
  {
    wchar_t status = imString[idxOfSep];
    if ( status == L'E' )
      InputMethodContext::get().enable();
    else
      InputMethodContext::get().disable();
  }

  return NS_OK;
}

template <typename KeyboardLayout>
NS_IMETHODIMP CTabImSwitchAppImpl<KeyboardLayout>::Init()
{
  if ( ! InputMethodContext::get().init() )
    return NS_ERROR_UNEXPECTED;

  return NS_OK;
}


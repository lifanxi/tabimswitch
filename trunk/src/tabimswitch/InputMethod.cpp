#pragma warning(disable:4512)  // assignment operator could not be generated
#pragma warning(disable:4127)  // conditional expression is constant

#include <windows.h>
#include <nsISupports.h>
#include <nsStringApi.h>

#include "InputMethod.h"
#include "NativeCodeLogger.h"
#include "SystemInputMethod.h"
#include "InputMethodContext.h"
#include "KeyboardLayout.h"

#include <sstream>

NS_IMPL_ISUPPORTS1(CInputMethod, IInputMethod)

CInputMethod::CInputMethod()
: m_initialized(false)
, m_enable(false)
, m_convMode(0)
, m_sentMode(0)
#ifdef  _USE_IME_FLAGS_FIXUP
, m_isConvModeAlphanum(FALSE)
, m_isSentModeNone(FALSE)
#else
, m_openStatus(FALSE)
#endif 
{
  LOGGER(LOG_TRACE) << "Created new CInputMethod object " << this << endlog;
}

CInputMethod::~CInputMethod()
{
  LOGGER(LOG_TRACE) << "Delete CInputMethod object " << this << endlog;
}

/* readonly attribute AString readableString; */
NS_IMETHODIMP CInputMethod::GetReadableString(nsAString & aReadableString)
{
  if ( ! m_initialized )
    return NS_OK;

  std::wostringstream oss;
  oss << (m_enable?'e':'d') << ';'
      << m_keyboardLayout.c_str() << ';'
#ifdef  _USE_IME_FLAGS_FIXUP
      << (m_isConvModeAlphanum?'A':'N') << ';'
      << (m_isSentModeNone?'N':'V') << ';'
#else
      << (m_openStatus?'O':'C') << ';'
#endif //  _USE_IME_FLAGS_FIXUP
      ;

  struct MaskMapItem { DWORD mask; char openTag; };
  static const MaskMapItem convModeMask[] = {
    {IME_CMODE_NATIVE, 'N'},
    {IME_CMODE_KATAKANA, 'K'},
    {IME_CMODE_LANGUAGE, 'L'},
    {IME_CMODE_FULLSHAPE, 'F'},
    {IME_CMODE_ROMAN, 'R'}, 
    {IME_CMODE_CHARCODE, 'C'},
    {IME_CMODE_HANJACONVERT, 'H'},
    {IME_CMODE_SOFTKBD, 'S'}, 
    {IME_CMODE_NOCONVERSION, 'n'},
    {IME_CMODE_EUDC, 'E'},
    {IME_CMODE_SYMBOL, 's'},
    {IME_CMODE_FIXED, 'f'}
  };
  static const MaskMapItem sentModeMask[] = {
    {IME_SMODE_PLAURALCLAUSE, 'C'},
    {IME_SMODE_SINGLECONVERT, 'S'},
    {IME_SMODE_AUTOMATIC, 'A'},
    {IME_SMODE_PHRASEPREDICT, 'P'},
    {IME_SMODE_CONVERSATION, 'c'}
  };

  for ( size_t i=0; i<_countof(convModeMask); ++i )
  {
    if ( (m_convMode & convModeMask[i].mask) == convModeMask[i].mask )
      oss << convModeMask[i].openTag;
  }

  oss << ';';

  for ( size_t i=0; i<_countof(sentModeMask); ++i )
  {
    if ( (m_sentMode & sentModeMask[i].mask) == sentModeMask[i].mask )
      oss << sentModeMask[i].openTag;
  }

  aReadableString = oss.str().c_str();

  return NS_OK;
}

/* void useCurrent (); */
NS_IMETHODIMP CInputMethod::UseCurrent()
{
  try
  {
    LOGGER(LOG_TRACE) << "Copy current input method information to " << this << endlog;
    SystemInputMethod& sysIME = SystemInputMethod::get();

    InputMethodContext& imc = sysIME.getInputMethodContext();
    m_enable = imc.isEnabled();

    KeyboardLayout& kbl = sysIME.getKeyboardLayout();
    m_keyboardLayout = kbl.getCurrent();

#ifndef _USE_IME_FLAGS_FIXUP
    imc.getIMEMode(m_convMode, m_sentMode);
    m_openStatus = imc.getOpenStatus();
#else //  _USE_IME_FLAGS_FIXUP
    imc.getIMEMode(m_convMode, m_sentMode, m_isConvModeAlphanum, m_isSentModeNone);
#endif //  _USE_IME_FLAGS_FIXUP

    m_initialized = true;

    return NS_OK;
  }
  catch ( std::exception& e )
  {
    LOGGER(LOG_DEBUG) << "Failed to use get Input Method state: " << e.what() << endlog;
  }

  return NS_ERROR_UNEXPECTED;
}

/* void setAsCurrent (); */
NS_IMETHODIMP CInputMethod::SetAsCurrent()
{
  LOGGER(LOG_TRACE) << "Try to use " << this << " as browser input method." << endlog;

  if ( ! m_initialized )
    return NS_ERROR_NOT_INITIALIZED;

  try
  {
    SystemInputMethod& sysIME = SystemInputMethod::get();

    InputMethodContext& imc = sysIME.getInputMethodContext();

    if ( m_enable )
      imc.enable();

    KeyboardLayout& kbl = sysIME.getKeyboardLayout();
    kbl.setCurrent(m_keyboardLayout);

#ifdef  _USE_IME_FLAGS_FIXUP
    imc.setIMEMode(m_convMode, m_sentMode, m_isConvModeAlphanum, m_isSentModeNone);
#else //  _USE_IME_FLAGS_FIXUP
    imc.setIMEMode(m_convMode, m_sentMode);
    imc.setOpenStatus(m_openStatus?true:false);
#endif //  _USE_IME_FLAGS_FIXUP

    if ( ! m_enable )
      imc.disable();

    return NS_OK;
  }
  catch (std::exception& e)
  {
    LOGGER(LOG_DEBUG) << "Failed to use set Input Method state: " << e.what() << endlog;
  }
  return NS_ERROR_UNEXPECTED;
}

/* attribute boolean enabled; */
NS_IMETHODIMP CInputMethod::GetEnabled(PRBool *aEnabled)
{
  if ( aEnabled == NULL )
    return NS_ERROR_NULL_POINTER;

  *aEnabled = (m_enable?PR_TRUE:PR_FALSE);

  return NS_OK;
}
NS_IMETHODIMP CInputMethod::SetEnabled(PRBool aEnabled)
{
  m_enable = (aEnabled?true:false);
  return NS_OK;
}

/* attribute AString keyboardLayout; */
NS_IMETHODIMP CInputMethod::GetKeyboardLayout(nsAString & aKeyboardLayout)
{
  aKeyboardLayout = m_keyboardLayout.c_str();
  return NS_OK;
}
NS_IMETHODIMP CInputMethod::SetKeyboardLayout(const nsAString & aKeyboardLayout)
{
  m_keyboardLayout = aKeyboardLayout.BeginReading();
  return NS_OK;
}

/* attribute boolean openStatus; */
NS_IMETHODIMP CInputMethod::GetOpenStatus(PRBool *aOpenStatus)
{
  if ( aOpenStatus == NULL )
    return NS_ERROR_NULL_POINTER;

  *aOpenStatus = (m_openStatus?PR_TRUE:PR_FALSE);
  return NS_OK;
}
NS_IMETHODIMP CInputMethod::SetOpenStatus(PRBool aOpenStatus)
{
  m_openStatus = (aOpenStatus!=PR_FALSE);

  return NS_OK;
}


/* attribute long convMode; */
NS_IMETHODIMP CInputMethod::GetConvMode(PRInt32 *aConvMode)
{
  if ( NULL == aConvMode )
    return NS_ERROR_NULL_POINTER;

  *aConvMode = m_convMode;

  return NS_OK;
}
NS_IMETHODIMP CInputMethod::SetConvMode(PRInt32 aConvMode)
{
  m_convMode = aConvMode;
  return NS_OK;
}

/* attribute long sentMode; */
NS_IMETHODIMP CInputMethod::GetSentMode(PRInt32 *aSentMode)
{
  if ( NULL == aSentMode )
    return NS_ERROR_NULL_POINTER;

  *aSentMode = m_sentMode;
  return NS_OK;
}
NS_IMETHODIMP CInputMethod::SetSentMode(PRInt32 aSentMode)
{
  m_sentMode = aSentMode;
  return NS_OK;
}


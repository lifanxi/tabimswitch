#ifndef _INPUT_METHOD_H_
#define _INPUT_METHOD_H_

#include "tabimswitch.h"
#include <string>

#define INPUTMETHOD_CONTRACTID "@tabimswitch.googlecode.com/inputmethod;1"
#define INPUTMETHOD_CLASSNAME "InputMethod"
/* 82ce76d2-2a63-47ca-8f13-361e2f97fc80 */
#define INPUTMETHOD_CID { 0x82ce76d2, 0x2a63, 0x47ca, \
  {0x8f, 0x13, 0x36, 0x1e, 0x2f, 0x97, 0xfc, 0x80} }

class CInputMethod : public IInputMethod
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_IINPUTMETHOD

  CInputMethod();

private:
  ~CInputMethod();

protected:
  bool m_initialized;

  std::wstring m_keyboardLayout;
  bool m_enable;
  DWORD m_convMode;
  DWORD m_sentMode;

#ifdef  _USE_IME_FLAGS_FIXUP
  BOOL m_isConvModeAlphanum;
  BOOL m_isSentModeNone;
#else
  BOOL m_openStatus;
#endif // def  _USE_IME_FLAGS_FIXUP

};

#endif // _INPUT_METHOD_H_

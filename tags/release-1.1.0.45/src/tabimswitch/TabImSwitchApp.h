#ifndef __TABIMSWITCH_APP_H__
#define __TABIMSWITCH_APP_H__

#include <memory>
#include "tabimswitch.h"

#define TABIMSWITCH_APP_CONTRACTID "@tabimswitch.googlecode.com/application;1"
#define TABIMSWITCH_APP_CLASSNAME "TabImSwitchApp"
/* 1efb2b80-1908-4dde-a9bd-ccadef225069 */
#define TABIMSWITCH_APP_CID { 0x1efb2b80, 0x1908, 0x4dde, \
  {0xa9, 0xbd, 0xcc, 0xad, 0xef, 0x22, 0x50, 0x69} }

class SystemInputMethod;

class CTabImSwitchApp : public ITabImSwitchApp
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_ITABIMSWITCHAPP

  CTabImSwitchApp();

private:
  ~CTabImSwitchApp();
  CTabImSwitchApp& operator=(CTabImSwitchApp const&);

private:
  void setInputMethodByCurrent(IInputMethod* im);
  SystemInputMethod& m_sysIME;
};

#endif // __TABIMSWITCH_APP_H__

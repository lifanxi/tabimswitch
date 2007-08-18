#ifndef __TABIMSWITCH_APP_MULTILINGUAL_IMPL_H__
#define __TABIMSWITCH_APP_MULTILINGUAL_IMPL_H__

#include <windows.h>
#include "tabimswitch.h"

#define TABIMSWITCH_APPML_CONTRACTID "@tabimswitch.googlecode.com/application;2"
#define TABIMSWITCH_APPML_CLASSNAME "TabImSwitchAppMultilingual"
/* 71b86ce3-f1c6-4762-b09f-35893ed91ee4 */
#define TABIMSWITCH_APPML_CID { 0x71b86ce3, 0xf1c6, 0x4762, \
  {0xb0, 0x9f, 0x35, 0x89, 0x3e, 0xd9, 0x1e, 0xe4} }

class CTabImSwitchAppMultilingual : public ITabImSwitchApp
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_ITABIMSWITCHAPP

  CTabImSwitchAppMultilingual();

private:
  ~CTabImSwitchAppMultilingual();

protected:
  /* additional members */
};

#endif // __TABIMSWITCH_APP_MULTILINGUAL_IMPL_H__

#ifndef __TABIMSWITCH_APP_IMPL_H__
#define __TABIMSWITCH_APP_IMPL_H__

#include <windows.h>
#include "tabimswitch.h"

#define TABIMSWITCH_APP_CONTRACTID "@tabimswitch.googlecode.com/application;1"
#define TABIMSWITCH_APP_CLASSNAME "TabImSwitchApp"
/* 1efb2b80-1908-4dde-a9bd-ccadef225069 */
#define TABIMSWITCH_APP_CID { 0x1efb2b80, 0x1908, 0x4dde, \
    {0xa9, 0xbd, 0xcc, 0xad, 0xef, 0x22, 0x50, 0x69} }

/* Header file */
class CTabImSwitchApp : public ITabImSwitchApp
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_ITABIMSWITCHAPP

  CTabImSwitchApp();

private:
  ~CTabImSwitchApp();

protected:
  /* additional members */
};

#endif // __TABIMSWITCH_APP_IMPL_H__

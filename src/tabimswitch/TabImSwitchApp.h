#ifndef __TABIMSWITCH_APP_H__
#define __TABIMSWITCH_APP_H__

#include "TabImSwitchAppImpl.h"
#include "KeyboardLayout.h"

#define TABIMSWITCH_APP_CONTRACTID "@tabimswitch.googlecode.com/application;1"
#define TABIMSWITCH_APP_CLASSNAME "TabImSwitchApp"
/* 1efb2b80-1908-4dde-a9bd-ccadef225069 */
#define TABIMSWITCH_APP_CID { 0x1efb2b80, 0x1908, 0x4dde, \
  {0xa9, 0xbd, 0xcc, 0xad, 0xef, 0x22, 0x50, 0x69} }

#define TABIMSWITCH_APPML_CONTRACTID "@tabimswitch.googlecode.com/application;2"
#define TABIMSWITCH_APPML_CLASSNAME "TabImSwitchAppMultilingual"
/* 71b86ce3-f1c6-4762-b09f-35893ed91ee4 */
#define TABIMSWITCH_APPML_CID { 0x71b86ce3, 0xf1c6, 0x4762, \
  {0xb0, 0x9f, 0x35, 0x89, 0x3e, 0xd9, 0x1e, 0xe4} }

typedef CTabImSwitchAppImpl<DefaultKeyboardLayout> CTabImSwitchApp;
typedef CTabImSwitchAppImpl<MultilingualKeyboardLayout> CTabImSwitchAppMultilingual;

#endif // __TABIMSWITCH_APP_H__

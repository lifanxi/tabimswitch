#ifndef _SYSTEM_INPUT_METHOD_MANAGER_H_
#define _SYSTEM_INPUT_METHOD_MANAGER_H_

#include "tabimswitch.h"

#define TABIMSWITCH_SYSIM_MANAGER_CONTRACTID "@tabimswitch.googlecode.com/system-manager;1"
#define TABIMSWITCH_SYSIM_MANAGER_CLASSNAME "SystemInputMethodManager"
 /* 5e5b3fe0-a674-4bc9-a818-02dc4824d74b */
#define TABIMSWITCH_SYSIM_MANAGER_CID { 0x5e5b3fe0, 0xa674, 0x4bc9, \
  {0xa8, 0x18, 0x02, 0xdc, 0x48, 0x24, 0xd7, 0x4b} }

class CSystemInputMethodManager : public ISystemInputMethodManager
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_ISYSTEMINPUTMETHODMANAGER

  CSystemInputMethodManager();

private:
  ~CSystemInputMethodManager();

protected:
  static const char SEPARATOR = ';';
  /* additional members */
};


#endif // _SYSTEM_INPUT_METHOD_MANAGER_H_

#ifndef __TABIMSWITCH_APP_IMPL_H__
#define __TABIMSWITCH_APP_IMPL_H__

#pragma warning(push)
# pragma warning(disable:4512)  // assignment operator could not be generated
# pragma warning(disable:4127)  // conditional expression is constant

#include <memory>
#include "tabimswitch.h"
#include "InputMethodContext.h"
#include <nsStringApi.h>
#include <Windows.h>

template <typename KeyboardLayout>
class CTabImSwitchAppImpl : public ITabImSwitchApp
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_ITABIMSWITCHAPP

  CTabImSwitchAppImpl();

private:
  ~CTabImSwitchAppImpl();

protected:
  typedef KeyboardLayout KeyboardLayoutType;
};

#include "TabImSwitchAppImpl.inl"

#pragma warning(pop)

#endif // __TABIMSWITCH_APP_IMPL_H__

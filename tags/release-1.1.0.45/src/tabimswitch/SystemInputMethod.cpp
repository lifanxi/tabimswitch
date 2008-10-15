#include <windows.h>
#include <assert.h>

#include <sstream>
#include <string>
#include <iomanip>
using std::setw;
using std::hex;
using std::setfill;

#include "SystemInputMethod.h"
#include "KeyboardLayout.h"
#include "InputMethodContext.h"
#include "NativeCodeLogger.h"

SystemInputMethod& SystemInputMethod::get(void)
{
  static SystemInputMethod s_sim;
  return s_sim;
}

SystemInputMethod::SystemInputMethod(void)
: m_keyboardLayoutManagerType(AutoKeyboardLayoutManager)
, m_keyboardLayout(KeyboardLayout::createByEnvironment())
, m_inputMethodContext(new InputMethodContext())
{
}

SystemInputMethod::~SystemInputMethod(void)
{

}

bool SystemInputMethod::isMultilingualEnvironment(void) const
{
  return m_keyboardLayout->isMultilingual();
}

void SystemInputMethod::useKeyboardLayoutManager(SystemInputMethod::KeyboardLayoutManagerType type)
{
  if ( m_keyboardLayoutManagerType != type )
  {
    switch ( type )
    {
    case AutoKeyboardLayoutManager:
      {
        LOGGER(LOG_DEBUG) << "Auto-detect multilingual environment." << endlog;
        m_keyboardLayout.reset(KeyboardLayout::createByEnvironment());
        break;
      }
    case DefaultKeyboardLayoutManager:
      {
        LOGGER(LOG_DEBUG) << "Force use non-multilingual version." << endlog;
        m_keyboardLayout.reset(KeyboardLayout::createDefault());
        break;
      }
    case MultilingualKeyboardLayoutManager:
      {
        LOGGER(LOG_DEBUG) << "Force use multilingual version." << endlog;
        m_keyboardLayout.reset(KeyboardLayout::createMultilingual());
        break;
      }
    default:
      assert(false);
    }

    m_keyboardLayoutManagerType = type;
  }
}

SystemInputMethod::KeyboardLayoutManagerType SystemInputMethod::getKeyboardLayoutManagerType(void) const
{
  return m_keyboardLayoutManagerType;
}

std::wstring SystemInputMethod::getInputMethodList(void) const
{
  HKL sysHKLs[64] = {0};
  int count = ::GetKeyboardLayoutList(_countof(sysHKLs), sysHKLs);

  std::wostringstream oss;
  for ( int i=0; i<count; ++i )
  {
    oss << hex << setw(8) << setfill(L'0') << sysHKLs[i] << SEPARATOR;
  }

  return oss.str();
}

wchar_t SystemInputMethod::getInputMethodListSeparator(void) const
{
  return SEPARATOR;
}

void SystemInputMethod::init(void)
{
  if ( ! m_inputMethodContext->init() )
    throw std::runtime_error("Failed to init input method context.");
  if ( ! m_keyboardLayout->init() )
    throw std::runtime_error("Failed to init keyboard layout.");
}


#ifndef __TABIMSWITCH_DEBUG_LOG_H__
#define __TABIMSWITCH_DEBUG_LOG_H__

#include <windows.h>
#include "tabimswitch.h"

#define TABIMSWITCH_DEBUG_LOGGER_CONTRACTID "@tabimswitch.googlecode.com/logger;1"
#define TABIMSWITCH_DEBUG_LOGGER_CLASSNAME "TabImSwitch Debug Logger"
/* ee3df72f-5427-4be3-bc08-db7523d945e7 */
#define TABIMSWITCH_DEBUG_LOGGER_CID {0xee3df72f, 0x5427, 0x4be3, \
  {0xbc, 0x08, 0xdb, 0x75, 0x23, 0xd9, 0x45, 0xe7} }

class FileLogger;

#pragma warning(push)
# pragma warning(disable:4512) // 'CDebugLogger' : assignment operator could not be generated

class CDebugLogger : public IDebugLogger
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_IDEBUGLOGGER

  CDebugLogger();

private:
  ~CDebugLogger();

protected:
  FileLogger& m_logger;
};

#pragma warning(pop)

#endif // __TABIMSWITCH_DEBUG_LOG_H__

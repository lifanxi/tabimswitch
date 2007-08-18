#include "NativeCodeLogger.h"
#include "LoggerImpl.h"

#include <windows.h>
#include <vector>

CNativeLoggerConfig& CNativeLoggerConfig::instance(void)
{
  static CNativeLoggerConfig config;
  return config;
}

void CNativeLoggerConfig::setLevel(LogLevel level)
{
  LOGGER(LOG_MUST_PRINT) << "Native log level changed to " << level << endlog;
  m_level = level;
}

CLineLogger::CLineLogger(LogLevel level, char const* srcfile, int srcline)
: m_level(level)
, m_srcFile(srcfile)
, m_srcLine(srcline)
, m_logFinished(false)
{
}

CLineLogger::EndLogLine_t endlog;

CLineLogger::~CLineLogger(void)
{
  if ( ! isLogFinished() )
    finishLog();
}

void CLineLogger::finishLog(void)
{
  assert ( ! isLogFinished() );

  FileLogger& logger = FileLogger::getLogger();
  if ( shouldPrint() )
    logger.write(m_srcFile, m_srcLine, m_level, m_message.str().c_str());

  m_logFinished = true;
}

CLineLogger& operator<<(CLineLogger& logger, wchar_t const*const wideStr)
{
  if ( wideStr != NULL )
  {
    int ansiLength = WideCharToMultiByte(CP_THREAD_ACP, 0, wideStr, -1, NULL, 0, NULL, NULL);
    if ( ansiLength )
    {
      std::vector<char> vAnsiString(ansiLength, '\0');
      int res = WideCharToMultiByte(CP_THREAD_ACP, 0, wideStr, -1, &vAnsiString[0], ansiLength, NULL, NULL);
      if ( res )
      {
        logger << &vAnsiString[0];
        return logger;
      }
    }

    logger << "(UNICODE conversion failed: " << ::GetLastError();
  }
  else
  {
    logger << "(null)";
  }

  return logger;
}

CLineLogger& operator<<(CLineLogger& logger, std::wstring const& wideStr)
{
  logger << wideStr.c_str();
  return logger;
}

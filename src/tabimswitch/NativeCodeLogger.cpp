#include "NativeCodeLogger.h"
#include "LoggerImpl.h"

#include <windows.h>
#include <vector>
#include <atlbase.h>
#include <atlconv.h>
#include <iomanip>
using std::setw;
using std::setfill;
using std::hex;

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

CLineLogger::CLineLoggerObj::CLineLoggerObj(void)
: m_logFinished(false)
{}

CLineLogger::CLineLogger(LogLevel level, char const* srcfile, int srcline)
: m_level(level)
, m_srcFile(srcfile)
, m_srcLine(srcline)
, m_obj(new CLineLoggerObj())
{
}

CLineLogger::EndLogLine_t endlog;

CLineLogger::~CLineLogger(void)
{
  if ( ! isLogFinished() )
    finishLog();
}

void CLineLogger::finishLog(void) const
{
  assert ( ! isLogFinished() );

  FileLogger& logger = FileLogger::getLogger();
  if ( shouldPrint() )
    logger.write(m_srcFile, m_srcLine, m_level, m_obj->m_message.str().c_str());

  m_obj->m_logFinished = true;
}

CLineLogger const& operator<<(CLineLogger const& logger, wchar_t const*const wideStr)
{
  if ( wideStr != NULL )
  {
    try
    {
      ATL::CW2A ansiString(wideStr);
      logger << static_cast<char const*>(ansiString);
    }
    catch (ATL::CAtlException& e)
    {
      logger << "(UNICODE conversion failure " << reinterpret_cast<void*>(e.m_hr) << ")";
    }
  }
  else
  {
    logger << "(null)";
  }

  return logger;
}

CLineLogger const& operator<<(CLineLogger const& logger, std::wstring const& wideStr)
{
  logger << wideStr.c_str();
  return logger;
}

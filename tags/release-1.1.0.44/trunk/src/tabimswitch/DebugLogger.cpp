# pragma warning(disable:4512)  // assignment operator could not be generated
# pragma warning(disable:4127)  // conditional expression is constant

#include "DebugLogger.h"
#include "LoggerImpl.h"
#include "NativeCodeLogger.h"
#include <nsStringApi.h>
#include <Windows.h>

NS_IMPL_ISUPPORTS1(CDebugLogger, IDebugLogger)

CDebugLogger::CDebugLogger()
: m_logger(FileLogger::getLogger())
{
  /* member initializers and constructor code */
}

CDebugLogger::~CDebugLogger()
{
  /* destructor code */
}

/* void init (in string logFile); */
NS_IMETHODIMP CDebugLogger::Init(const char *logFile)
{
  if ( logFile != NULL )
  {
    m_logger.setOutput(logFile);
  }

  return NS_OK;
}

/* attribute long logLevel; */
NS_IMETHODIMP CDebugLogger::GetLogLevel(PRInt32 *aLogLevel)
{
  if ( NULL == aLogLevel )
    return NS_ERROR_NULL_POINTER;

  *aLogLevel = static_cast<int>(CNativeLoggerConfig::instance().getLevel());
  return NS_OK;
}

NS_IMETHODIMP CDebugLogger::SetLogLevel(PRInt32 aLogLevel)
{
  if ( aLogLevel < LOG_MUST_PRINT )
    aLogLevel = LOG_MUST_PRINT;

  if ( aLogLevel > LOG_TRACE )
    aLogLevel = LOG_TRACE;

  CNativeLoggerConfig::instance().setLevel(static_cast<LogLevel>(aLogLevel));

  return NS_OK;
}

/* void writeLog (in long level, in string message); */
NS_IMETHODIMP CDebugLogger::WriteLog(PRInt32 level, const char *message)
{
  if ( NULL != message )
  {
    m_logger.write(NULL, 0, level, message);
  }
  return NS_OK;
}

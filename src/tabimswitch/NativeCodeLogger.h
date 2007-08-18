#ifndef _NATIVVE_CODE_LOGGER_H_
#define _NATIVVE_CODE_LOGGER_H_

#include <string>
#include <sstream>
#include <assert.h>

#include "Uncopyable.h"

enum LogLevel
{
  LOG_MUST_PRINT = 0,
  LOG_ERROR,
  LOG_WARN,
  LOG_INFO,
  LOG_DEBUG,
  LOG_TRACE,
};

class CNativeLoggerConfig
  : public Uncopyable
{
public:
  static CNativeLoggerConfig& instance(void);

  LogLevel getLevel(void) { return m_level; }
  void setLevel(LogLevel level);

private:
  CNativeLoggerConfig(void): m_level(LOG_WARN) {}

private:
  LogLevel m_level;
};

class CLineLogger
  : public Uncopyable
{
public:
  CLineLogger(LogLevel level, char const* srcfile, int srcline);
  ~CLineLogger(void);
  void finishLog();
  bool isLogFinished(void) const { return m_logFinished; }
  bool shouldPrint(void) const { return m_level <= CNativeLoggerConfig::instance().getLevel(); }

  struct EndLogLine_t{};

private:
  std::ostringstream m_message;
  LogLevel m_level;
  char const* const m_srcFile;
  int m_srcLine;
  bool m_logFinished;

private:
  template < typename value_t>
  friend CLineLogger& operator<<(CLineLogger& logger, value_t const& value);
  friend CLineLogger& operator<<(CLineLogger& logger, EndLogLine_t const&);
};

CLineLogger& operator<<(CLineLogger& logger, wchar_t const*const wideStr);
CLineLogger& operator<<(CLineLogger& logger, std::wstring const& wideStr);

template < typename value_t>
inline
CLineLogger& operator<<(CLineLogger& logger, value_t const& value)
{
  assert ( ! logger.isLogFinished() );
  if ( logger.shouldPrint() )
    logger.m_message << value;

  return logger;
}

inline
CLineLogger& operator<<(CLineLogger& logger, CLineLogger::EndLogLine_t const&)
{
  logger.finishLog();
  assert ( logger.isLogFinished() );
  return logger;
}

extern CLineLogger::EndLogLine_t endlog;

#define MAKE_LOGGER_NAME(uniq) __XLOGGER_##uniq

#define LOGGER(level) CLineLogger MAKE_LOGGER_NAME(__LINE__)(level, __FILE__, __LINE__); MAKE_LOGGER_NAME(__LINE__)
#define DEF_LOGGER(level, var) CLineLogger var(level, __FILE__, __LINE__)

#endif // _NATIVVE_CODE_LOGGER_H_

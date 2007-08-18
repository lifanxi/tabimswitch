#ifndef _NATIVVE_CODE_LOGGER_H_
#define _NATIVVE_CODE_LOGGER_H_

#include <string>
#include <sstream>
#include <memory>
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
  void finishLog() const;
  bool isLogFinished(void) const { return m_obj->m_logFinished; }
  bool shouldPrint(void) const { return m_level <= CNativeLoggerConfig::instance().getLevel(); }

  struct EndLogLine_t{};

private:
  struct CLineLoggerObj
  {
    CLineLoggerObj(void);
    std::ostringstream m_message;
    bool m_logFinished;
  };

  std::auto_ptr<CLineLoggerObj> m_obj;
  LogLevel const m_level;
  char const* const m_srcFile;
  int const m_srcLine;

private:
  template < typename value_t>
  friend CLineLogger const& operator<<(CLineLogger const& logger, value_t const& value);
  friend CLineLogger const& operator<<(CLineLogger const& logger, EndLogLine_t const&);
};

CLineLogger const& operator<<(CLineLogger const& logger, wchar_t const*const wideStr);
CLineLogger const& operator<<(CLineLogger const& logger, std::wstring const& wideStr);

template < typename value_t>
inline
CLineLogger const& operator<<(CLineLogger const& logger, value_t const& value)
{
  assert ( ! logger.isLogFinished() );
  if ( logger.shouldPrint() )
    logger.m_obj->m_message << value;

  return logger;
}

inline
CLineLogger const& operator<<(CLineLogger const& logger, CLineLogger::EndLogLine_t const&)
{
  logger.finishLog();
  assert ( logger.isLogFinished() );
  return logger;
}

extern CLineLogger::EndLogLine_t endlog;

#define LOGGER(level) CLineLogger(level, __FILE__, __LINE__)
#define DEF_LOGGER(level, var) CLineLogger var(level, __FILE__, __LINE__)

#endif // _NATIVVE_CODE_LOGGER_H_

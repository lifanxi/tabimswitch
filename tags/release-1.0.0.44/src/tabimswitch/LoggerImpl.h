#ifndef _LOGGER_IMPL_H_
#define _LOGGER_IMPL_H_

#include <string>
#include <fstream>
#include <windows.h>
#include <process.h>

class LoggerLock
{
public:
  LoggerLock() { InitializeCriticalSection(&m_lock); }
  ~LoggerLock() { DeleteCriticalSection(&m_lock); }
  void lock() { EnterCriticalSection(&m_lock); }
  void unlock() { LeaveCriticalSection(&m_lock); }

protected:
  CRITICAL_SECTION m_lock;
};

template < typename lock_t >
class Locker
{
public:
  Locker(lock_t& lock) : m_lock(lock) { lock.lock(); }
  ~Locker() { m_lock.unlock(); }

private:
  Locker& operator=(Locker<lock_t> const&);
  Locker(Locker<lock_t> const&);

private:
  lock_t& m_lock;
};

typedef Locker<LoggerLock> LoggerLocker;

class FileLogger
{
public:
  ~FileLogger(void);
  static FileLogger& getLogger(void);

private:
  FileLogger(void);

public:
  void setOutput(std::string const& fileName);
  void write(const char*const srcfile, unsigned int srcline, int level, const char*const message);

private:
  char const*const getShortSrcFile(char const*const srcfile);

private:
  std::ofstream m_output;;
  std::string m_filename;
  LoggerLock m_lock;
};

#endif // _LOGGER_IMPL_H_

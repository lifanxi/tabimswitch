#include "LoggerImpl.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <assert.h>

#include <Shlwapi.h>
#pragma comment(lib, "shlwapi")

using std::setw;
using std::setfill;
using std::endl;

FileLogger::FileLogger(void)
{
  CHAR szTempFolder[MAX_PATH] = {0};
  CHAR szTempFile[MAX_PATH] = {0};
  ::GetTempPathA(sizeof(szTempFolder), szTempFolder);
  if ( ::GetTempFileNameA(szTempFolder, "tabimswitch-log", 0, szTempFile) )
  {
    setOutput(szTempFile);
  }
}

FileLogger::~FileLogger(void)
{
}

FileLogger& FileLogger::getLogger(void)
{
  static FileLogger gsLogger;
  return gsLogger;
}

void FileLogger::setOutput(std::string const& fileName)
{
  if ( 0 == _stricmp(m_filename.c_str(), fileName.c_str()) )
    return;

  if ( ! fileName.empty() )
  {
    if ( ! m_filename.empty() )
    {
      m_output.close();
      ::MoveFileExA(m_filename.c_str(), fileName.c_str(),
          MOVEFILE_COPY_ALLOWED|MOVEFILE_REPLACE_EXISTING);
    }
    else
    {
      if ( ::PathFileExistsA(fileName.c_str() ) )
        ::DeleteFileA(fileName.c_str());
    }

    m_output.open(fileName.c_str(), std::ios::out|std::ios::app);

    if ( m_output )
    {
      std::ostringstream oss;
      oss << "Copy log file from " << m_filename.c_str() << " to " << fileName.c_str();
      write(__FILE__, __LINE__, 0, oss.str().c_str());

      m_filename = fileName;
    }
    else
    {
      m_filename.clear();
    }
  }
}

char const*const FileLogger::getShortSrcFile(char const*const srcfile)
{
  assert ( srcfile != NULL );
  char const* srcshortname = srcfile;
  if ( NULL != srcfile && 0 != *srcfile )
  {
    srcshortname = strrchr(srcfile, '\\');;
    if ( NULL != srcshortname )
      srcshortname++;
    else
      srcshortname = srcfile;
  }
  else
  {
    return "";
  }

  return srcshortname;
}

void FileLogger::write(const char*const srcfile, unsigned int srcline,
                       int level, const char*const message)
{
  std::ostream& output = (m_output ? m_output : std::cout);

  SYSTEMTIME st;
  GetLocalTime(&st);

  LoggerLocker locker(m_lock);

  output << st.wYear << '-' 
    << setw(2) << setfill('0') << st.wMonth << '-'
    << setw(2) << setfill('0') << st.wDay << ' '
    << setw(2) << setfill('0') << st.wHour << ':'
    << setw(2) << setfill('0') << st.wMinute << ':'
    << setw(2) << setfill('0') << st.wSecond << '.' 
    << setw(3) << setfill('0') << st.wMilliseconds << '\t'
    << '[' << ::GetCurrentProcessId() << ':' << ::GetCurrentThreadId() << "]\t"
    << level << '\t' << message << '\t';

  if ( srcfile )
    output << "[N:" << getShortSrcFile(srcfile) << '(' << srcline << ")]";
  else
    output << "[JS]";

  output << endl;
}

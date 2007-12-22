@echo off
setlocal

set mydir=%~dp0
set version_file=%mydir%\..\buildversion

if "%1"=="" (
  if not exist "%version_file%" (
    echo File %version_file% does not exist.
    echo Please use %~n0.cmd [version]
    goto usage
  )
  for /f %%l in (%version_file%) do (
    set _ver=%%l
  )
) else (
  set _ver=%1
)

echo Tag source code for version %_ver%...

set _pending_op=undefined
for /f %%c in ('svn status ^| grep "^[MADR!]" ^| wc -l') do (
  set _pending_op=%%c
)

if not "%_pending_op%"=="0" (
:ask_checkin
  set /p _checkin=There are %_pending_op% code change pending, will you check in now [yes/no]?
  if "%_checkin%"=="yes" (
    svn ci -m "Check in the last code change for release %_ver%"
  ) else if "%_checkin%"=="no" (
    goto finish
  ) else (
    goto ask_checkin
  )
)

svn copy -m "Create release tag release-%_ver%" https://tabimswitch.googlecode.com/svn/trunk https://tabimswitch.googlecode.com/svn/tags/release-%_ver%

:finish

@echo off
setlocal

if "%1"=="" (
  call "%~dp0get-cur-version.cmd"
  if errorlevel 1 goto usage
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
    goto error
  ) else (
    goto ask_checkin
  )
)

svn copy -m "Create release tag release-%_ver%" https://tabimswitch.googlecode.com/svn/trunk https://tabimswitch.googlecode.com/svn/tags/release-%_ver%
if errorlevel 1 goto error

goto finish

:usage
echo %~n0.cmd [version]
echo.
exit /b 1

:error
echo Error execute command.
exit /b 2

:finish
exit /b 0

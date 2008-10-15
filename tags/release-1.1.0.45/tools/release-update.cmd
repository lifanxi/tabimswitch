@echo off
setlocal

if "%1"=="" (
  call "%~dp0get-cur-version.cmd"
  if errorlevel 1 goto usage
) else (
  set _ver=%1
)

svn rm -m "Delete old update RDF"  https://tabimswitch.googlecode.com/svn/update/update.rdf
if errorlevel 1 goto error

svn copy -m "Use latest update RDF" https://tabimswitch.googlecode.com/svn/tags/release-%_ver%/update/update.rdf https://tabimswitch.googlecode.com/svn/update/update.rdf
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

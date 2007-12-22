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

svn rm -m "Delete old update RDF"  https://tabimswitch.googlecode.com/svn/update/update.rdf
svn copy -m "Use latest update RDF" https://tabimswitch.googlecode.com/svn/tags/release-%_ver%/update/update.rdf https://tabimswitch.googlecode.com/svn/update/update.rdf

:usage
echo %~n0.cmd [version]
echo.

:finish

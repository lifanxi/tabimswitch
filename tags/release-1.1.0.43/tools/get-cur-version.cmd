@echo off

set mydir=%~dp0
set version_file=%mydir%\..\buildversion

if not exist "%version_file%" (
  echo File %version_file% does not exist.
  echo Please use %~n0.cmd [version]
  exit /b 1
)
for /f %%l in (%version_file%) do (
  set _ver=%%l
)


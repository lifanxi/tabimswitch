@echo off
setlocal

if "%1"=="" (
  call "%~dp0get-cur-version.cmd"
  if errorlevel 1 goto usage
) else (
  set _ver=%1
)

googlecode_upload -s "[Release] TabImSwitch %_ver%" -p tabimswitch -u ftofficer.zhangc -l "Featured Firefox Extension Type-Installer OpSys-Windows" output\tabimswitch-%_ver%.xpi
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

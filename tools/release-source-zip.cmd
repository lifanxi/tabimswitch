@echo off
setlocal

if "%1"=="" (
  call "%~dp0get-cur-version.cmd"
  if errorlevel 1 goto usage
) else (
  set _ver=%1
)

set _dir=tabimswitch-%_ver%
set _pkg=tabimswitch-%_ver%-src.zip

echo Build source code package for version %_ver%...

svn checkout http://tabimswitch.googlecode.com/svn/tags/release-%_ver% %_dir%
if errorlevel 1 (
  echo Checkout failed.
  goto finish
)

"D:\Program Files\7-Zip\7z.exe" a %_pkg% -xr0!*.svn* %_dir%
googlecode_upload -s "[Source Code] TabImSwitch %_ver% Source Code" -p tabimswitch -u ftofficer.zhangc -l "Type-Source" %_pkg%
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

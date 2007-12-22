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

set _dir=tabimswitch-%_ver%
set _pkg=tabimswitch-%_ver%-src.zip

echo Build source code package for version %_ver%...

svn checkout http://tabimswitch.googlecode.com/svn/tags/release-%_ver% %_dir%
if errorlevel 1 (
  echo Checkout failed.
  goto finish
)

rem for /f %%i in ('find %_dir% -iname .svn') do rmdir /s /q %%i
"D:\Program Files\7-Zip\7z.exe" a %_pkg% -xr0!*.svn* %_dir%
googlecode_upload -s "[Source Code] TabImSwitch %_ver% Source Code" -p tabimswitch -u ftofficer.zhangc -l "Type-Source" %_pkg%

goto finish

:usage
echo %~n0.cmd [version]
echo.

:finish
if exist "%_dir%" rmdir /s /q "%_dir%"
if exist "%_pkg%" del "%_pkg%"

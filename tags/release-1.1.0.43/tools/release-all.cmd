
setlocal

call "%~dp0tag-release.cmd" %1
if errorlevel 1 goto error

call "%~dp0release-xpi.cmd" %1
if errorlevel 1 goto error

call "%~dp0release-source-zip.cmd" %1
if errorlevel 1 goto error

call "%~dp0release-update.cmd" %1
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

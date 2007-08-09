if exist reg-comp-config.cmd call reg-comp-config.cmd

..\gecko-sdk\bin\regxpcom.exe -x "%cd%\..\..\components" -c "%_firefox_profile_path%\compreg.dat" -d "%_firefox_profile_path%\xpti.dat"
del "%_firefox_profile_path%\compreg.dat"
del "%_firefox_profile_path%\xpti.dat"
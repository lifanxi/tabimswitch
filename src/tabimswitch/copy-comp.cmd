copy ".\%1\tabimswitch.dll" "..\..\components\tabimswitch.dll"
copy ".\tabimswitch.xpt" "..\..\components\tabimswitch.xpt"
..\gecko-sdk\bin\regxpcom.exe -x "%cd%\..\..\components" -c "%_firefox_profile_path%\compreg.dat" -d "%_firefox_profile_path%\xpti.dat"
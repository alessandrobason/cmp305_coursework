cd /d %~dp0
powershell -Command "Invoke-WebRequest https://github.com/alessandrobason/cmp301_libraries/archive/refs/heads/master.zip -OutFile GenLibs.zip"
powershell Expand-Archive GenLibs.zip 
rmdir lib/S /Q
rmdir x64/S /Q
move /Y GenLibs\cmp301_libraries-master\lib %~dp0
move /Y GenLibs\cmp301_libraries-master\x64 %~dp0
del GenLibs.zip
rmdir GenLibs /S /Q
pause

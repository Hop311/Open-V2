
@echo off

set /p configuration=Choose "Debug" (default) or "Release": 
if not defined configuration set configuration=Debug

set args=/property:Configuration=%configuration% /maxCpuCount:2

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

msbuild "Open V2.sln" /t:container_generator;gui_window_generator;parser_generator %args%
msbuild "Open V2.sln" /t:file_explorer %args%

pause

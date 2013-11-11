@echo off

set output="out"

if not exist %output% (
	md %output%
	goto :MAKE
) else (
	for /F %%i in ('dir /b "%output%\*.*"') do (
		rd %output% /s /q
		md %output%
		goto :MAKE
	)	
)

:MAKE
ARM\bin\cs-make.exe %1 %2 %3 %4
echo:
pause
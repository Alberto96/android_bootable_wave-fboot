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
copy src\FOTAs8500.c src\FOTA.c
ARM\bin\cs-make.exe %1 %2 %3 %4
del src\FOTA.c
echo:
pause
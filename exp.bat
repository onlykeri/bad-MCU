@echo off&setlocal enabledelayedexpansion&mode | find "COM" > tmp.txt&for /f "delims=" %%i in (tmp.txt) do (set a=%%i&set a=!a: =!&set a=!a:~4,-1!&echo !a! > mode.tmp)&del tmp.txt

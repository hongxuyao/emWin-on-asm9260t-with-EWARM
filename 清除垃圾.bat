@echo off

::删除所有子目录中的TempFiles目录
for /f "tokens=*" %%a in ('dir /ad/b/s^|find /i "TempFiles"') do rd /q/s "%%a"

::删除所有子目录中的settings目录
for /f "tokens=*" %%a in ('dir /ad/b/s^|find /i "settings"') do rd /q/s "%%a"

::递归删除所有子目录中的*CustomSfr.sfr文件
del /f /s /q *CustomSfr.sfr

::递归删除所有子目录中的*.dep文件
del /f /s /q *.dep

::递归删除所有子目录中的*.out文件
del /f /s /q *.out
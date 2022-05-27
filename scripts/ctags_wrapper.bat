@echo off
set workdir=%CD%\
set YALTA=%~dp0bin\yalta.exe
%~dp0bin\ctags.exe --exclude=*.lua %* || exit /b 1
call :get_target %*
if "%TARGET%"=="." set TARGET=*
if "%TARGET%"=="*" (
    for /R %%G in (*.lua) do call :index_relative "%%~G" >> tags
) else call :index_lua "%TARGET%" >> tags

exit /b %errorlevel%

:get_target
    set "TARGET=%~1"
    shift
    if not "%~1"=="" goto get_target
exit /b

:index_relative
    set fname=%~dpnx1
    SETLOCAL ENABLEDELAYEDEXPANSION
    %YALTA% "!fname:%workdir%=!"
    SETLOCAL DISABLEDELAYEDEXPANSION
exit /b

:index_lua
    if x%~x1==x.lua %YALTA% "%~1"
    if x%~x1==x.LUA %YALTA% "%~1"
exit /b

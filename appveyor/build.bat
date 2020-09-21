echo on
md %BUILD_ROOT%
cd %BUILD_ROOT% || exit /b 1
cmake -G "%CMAKE_GENERATOR%" -DYALTA_VERSION_MAJOR=%YALTA_VERSION_MAJOR% -DYALTA_VERSION_MINOR=%YALTA_VERSION_MINOR% -DYALTA_BUILD=%APPVEYOR_BUILD_NUMBER% "%APPVEYOR_BUILD_FOLDER%" || exit /b 1
cmake --build . --config %CONFIGURATION% || exit /b 1

appveyor DownloadFile https://github.com/universal-ctags/ctags-win32/releases/download/%CTAGS_UTIL_DATE%/%CTAGS_UTIL_REV%/ctags-%CTAGS_UTIL_DATE%_%CTAGS_UTIL_REV%-%ARCH%.zip -FileName ctags.utility.zip || exit /b 1
set TEMP_DIR=%BUILD_ROOT%\temp
md %TEMP_DIR% || exit /b 1
7z x %BUILD_ROOT%\ctags.utility.zip -o%TEMP_DIR% || exit /b 1
move %TEMP_DIR%\ctags.exe %CONFIGURATION%\%PRODUCT_DIR%\bin || exit /b 1
move %TEMP_DIR%\license %CONFIGURATION%\%PRODUCT_DIR%\bin || exit /b 1

cd %CONFIGURATION% || exit /b 1
7z a yalta-%APPVEYOR_BUILD_VERSION%-%ARCH%.zip %PRODUCT_DIR% || exit /b 1
move *.zip "%APPVEYOR_BUILD_FOLDER%" || exit /b 1

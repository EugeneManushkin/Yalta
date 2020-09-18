@echo off
set INDEXER=%~dp0bin\yalta.exe
set TARGET="%~dpnx1"
if x%1==x set TARGET="%CD%"
echo !_TAG_FILE_FORMAT	2	/extended format; --format=1 will not append ;" to lines/
echo !_TAG_FILE_SORTED	0	/0=unsorted, 1=sorted, 2=foldcase/
echo !_TAG_OUTPUT_MODE	u-ctags	/u-ctags or e-ctags/
echo !_TAG_PROGRAM_AUTHOR	Eugene Manushkin	//
echo !_TAG_PROGRAM_NAME	Yalta	//
echo !_TAG_PROGRAM_URL	https://github.com/EugeneManushkin/Yalta	/official site/
echo !_TAG_PROGRAM_VERSION	0	//
if exist %TARGET%\* (
    cd %TARGET%
    (for /R %%G in (*.lua) do @echo %%G) | %INDEXER%
) else %INDEXER% %TARGET%

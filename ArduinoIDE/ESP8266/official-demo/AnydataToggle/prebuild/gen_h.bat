@Echo off
cls
echo.
echo.
echo -----------------------------------------
echo   JSON to .h  header files converter
echo -----------------------------------------
echo.
md temp >nul
echo.
echo .........................................
for %%f in (json/*.*) do (
  echo Processing: %%f
  copy json\%%f temp\%%~nf.j >nul
  bin\echo -n -e \0000 >> temp\%%~nf.j
  bin\xxd -i temp\%%~nf.j > temp\%%~nf.x
  bin\sed "s/unsigned char temp_%%~nf\_j\[\]\ =/const char %%~nf_json[] =/" temp\%%~nf.x | sed "s/unsigned int temp_%%~nf\_j\_len/const int %%~nf\_len/" > ..\%%~nf.h
  echo Resulting : %%~nf.h
  echo .........................................
)
echo.
del /Q /S temp\*.j >nul
del /Q /S temp\*.x >nul
rd temp >nul
echo -----------------------------------------
echo                 Done.
echo -----------------------------------------

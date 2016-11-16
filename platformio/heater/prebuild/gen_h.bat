@Echo off
cd prebuild
md temp >nul
for %%f in (json/*.*) do (
  echo --------------------------
  echo Processing: %%f
  copy json\%%f temp\%%~nf.j >nul
  bin\echo -n -e \0000 >> temp\%%~nf.j
  bin\xxd -i temp\%%~nf.j > temp\%%~nf.x
  bin\sed "s/unsigned char temp_%%~nf\_j\[\]\ =/const char %%~nf_json[] =/" temp\%%~nf.x | sed "s/unsigned int temp_%%~nf\_j\_len/const int %%~nf\_len/" > ..\lib\widgets\%%~nf.h
  echo Resulting : %%~nf.h
)
rem del /Q /S temp\*.j >nul
rem del /Q /S temp\*.x >nul
rd temp >nul
cd ..

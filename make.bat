
cd bin
del AsciiTetris.exe
cd ..
cl /Fe"bin/AsciiTetris" user32.lib src/*.cpp /EHsc /I include
move *.obj obj
REM "bin/AsciiTetris.exe"


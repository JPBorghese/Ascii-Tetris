
cd bin
del Tetris.exe
cd ..
cl /Fe"bin/Tetris" user32.lib src/*.cpp /EHsc /I include
move *.obj obj
REM "bin/Tetris.exe"


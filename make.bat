
cd bin
del Tetris.exe
cd ..
cl /Fe"bin/Tetris" user32.lib src/*.cpp /EHsc
move *.obj obj
"bin/Tetris.exe"


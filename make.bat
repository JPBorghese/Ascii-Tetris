
cd bin
del Tetris.exe
cd..
cl user32.lib src/*.cpp /EHsc
move *.obj obj
move *.exe bin
"bin/Tetris.exe"

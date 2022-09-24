@echo off
mkdir build
pushd build
g++ ..\%1.cpp -IC:\SDL2\include -LC:\SDL2\lib -Wall -lmingw32 -lSDL2_ttf -lSDL2main -lSDL2 -o %1.exe 
.\%1.exe
popd

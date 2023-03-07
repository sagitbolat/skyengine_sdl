@echo off
mkdir %1\build
pushd %1\build
g++ ..\%2.cpp -IC:\SDL2\include -LC:\SDL2\lib -Wall -lmingw32 -lSDL2main -lSDL2 -o %2.exe 
.\%2.exe
popd

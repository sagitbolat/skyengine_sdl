@echo off
mkdir build
pushd build
g++ ..\main.cpp -IC:\SDL2\include -LC:\SDL2\lib -Wall -lmingw32 -lSDL2main -lSDL2 -o main.exe 
.\main.exe
popd

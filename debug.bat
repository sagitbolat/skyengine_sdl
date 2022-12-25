@echo off
mkdir debug
pushd debug
g++ -g ..\%1.cpp -IC:\SDL2\include -LC:\SDL2\lib -Wall -lmingw32 -lSDL2main -lSDL2 
gdb a.exe
popd

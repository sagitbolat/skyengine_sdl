@echo off
mkdir %1\build
pushd %1\build
g++ ..\%2.cpp -IC:\SDL2\include -IC:\glew\include -IC:\glm -LC:\SDL2\lib -LC:\glew\lib -Wall -lmingw32 -lSDL2main -lSDL2 -lglew32 -lopengl32 -o %2.exe 
.\%2.exe
popd

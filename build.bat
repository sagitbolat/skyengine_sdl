@echo off
mkdir %1\build_win

if "%3" == ""       (GOTO CompileGame)
if "%3" == "-f"     (GOTO CompileUI)
if "%3" == "-ui"    (GOTO CompileUI)
if "%3" == "-gl"    (GOTO CompileGL)
if "%3" == "-al"    (GOTO CompileAL)

:CompileUI
    g++ -c Engine\UI\sky_ui_imgui.cpp -IC:\SDL2\include -IC:\glew\include -IC:\glm -o sky_ui_imgui.o -lmingw32 -lSDL2main -lSDL2 -lglew32 -lopengl32 -static-libgcc -static-libstdc++ 
    move .\sky_ui_imgui.o .\%1\build_win
    if "%3" == "-f" (GOTO CompileGL) else (GOTO CompileGame)
:CompileGL
    g++ -c Engine\OpenGL_renderer2D.cpp -IC:\SDL2\include -IC:\glew\include -IC:\glm -o OpenGL_renderer2D.o -lmingw32 -lSDL2main -lSDL2 -lglew32 -lopengl32 -static-libgcc -static-libstdc++ 
    move .\OpenGL_renderer2D.o .\%1\build_win
    XCOPY Engine\assets\shader.vs %1\build_win /D /Y
    XCOPY Engine\assets\shader.fs %1\build_win /D /Y
    if "%3" == "-f" (GOTO CompileAL) else (GOTO CompileGame)
:CompileAL
    g++ -c Engine\OpenAL_sound.cpp -IC:\SDL2\include -IC:\glew\include -IC:\OpenAL_SDK\include -o OpenAL_sound.o -lmingw32 -lSDL2main -lSDL2 -lglew32 -lopengl32 -lOpenAL32 -static-libgcc -static-libstdc++ 
    move .\OpenAL_sound.o .\%1\build_win

XCOPY Engine\assets\startup.png %1\build_win /D /Y 
XCOPY Engine\assets\glew32.dll %1\build_win /D /Y 
XCOPY Engine\assets\SDL2.dll %1\build_win /D /Y

:CompileGame
pushd %1\build_win
g++ OpenGL_renderer2D.o OpenAL_sound.o sky_ui_imgui.o ..\%2.cpp -IC:\SDL2\include -IC:\glew\include -IC:\glm -IC:\OpenAL_SDK\include -LC:\SDL2\lib -LC:\glew\lib -LC:\OpenAL_SDK\libs\Win64 -Wall -lmingw32 -lSDL2main -lSDL2 -lglew32 -lopengl32 -lOpenAL32 -static-libgcc -static-libstdc++ -o %2.exe
popd

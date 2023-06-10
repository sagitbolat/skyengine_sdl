@echo off
mkdir %1\build_win

pushd %1\build_win
g++ ..\..\Engine\UI\sky_ui_imgui.cpp ..\..\Engine\OpenGL_renderer2D.cpp ..\..\Engine\OpenAL_sound.cpp ..\%2.cpp -IC:\SDL2\include -IC:\glew\include -IC:\glm -IC:\OpenAL_SDK\include -LC:\SDL2\lib -LC:\glew\lib -LC:\OpenAL_SDK\libs\Win64 -Wall -lmingw32 -lSDL2main -lSDL2 -lglew32 -lopengl32 -lOpenAL32 -o %2.exe 
.\%2.exe
popd

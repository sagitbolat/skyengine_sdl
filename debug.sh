mkdir ./$1/debug
pushd ./$1/debug
    g++ -Wall -Werror -g ../$2.cpp -w -lSDL2 -lGLEW  -lGL
    gdb a.out
popd

mkdir ./$1/build
pushd ./$1/build
g++ -Wall -Werror -g ../$2.cpp -w -lSDL2 -lGLEW  -lGL
valgrind ./a.out
popd

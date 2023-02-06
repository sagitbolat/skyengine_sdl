mkdir build
pushd build
g++ ../$1.cpp -Wall -lSDL2 -I PWD -o $1
./$1
popd

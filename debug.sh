mkdir debug
pushd debug
g++ -Wall -g ../$1.cpp -w -lSDL2 
gdb a.out
layout src
popd

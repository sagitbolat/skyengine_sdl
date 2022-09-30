mkdir debug
pushd debug
g++ -Wall -Werror -g ../$1.cpp -w -lSDL2 
valgrind ./a.out
popd

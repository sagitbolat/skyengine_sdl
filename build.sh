mkdir ./$1/build
pushd ./$1/build
    g++ ../$2.cpp -Wall -lSDL2 -I PWD -o $2
    ./$2
popd

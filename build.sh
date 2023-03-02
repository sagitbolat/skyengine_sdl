mkdir build
pushd build
if [ $1 = '-ex' ]
then 
    g++ ../Examples/$2.cpp -Wall -lSDL2 -I PWD -o $2
    ./$2
elif [ $1 = '-f' ]
then
    g++ ../$2/$3.cpp -Wall -lSDL2 -I PWD -o $3
    ./$3
else 
    g++ ../$1.cpp -Wall -lSDL2 -I PWD -o $1
    ./$1
fi
popd

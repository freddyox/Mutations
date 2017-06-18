#!/bin/bash

echo "Compiling..."
echo " "
cd src/

g++ -c main.cpp HandleInput.cpp Mutations.cpp -pthread -m64 -std=gnu++11 -I/home/obrecht/Documents/ROOT6/root/include -L/home/obrecht/Documents/ROOT6/root/lib -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic


echo "Linking..."
echo " "

if [ -e main.cpp~ ];
then
    rm main.cpp~
fi

mv *.o ../linkers
cd ../linkers

g++ main.o HandleInput.o Mutations.o -o mutate -lsfml-graphics -lsfml-window -lsfml-system -I/home/obrecht/Documents/ROOT6/root/include -L/home/obrecht/Documents/ROOT6/root/lib -lCore -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic

mv mutate ../
cd ../

if [ -e compile.sh~ ] ;
then 
    rm compile.sh~ 
fi

cd include/
if [ -e *.hh~ ];
then
    rm *.hh~
fi

cd ..
echo "Executing..."
echo " "
./mutate


#!/bin/bash
#qt dir
export Qt5_DIR=$1
#clone nodes
git clone git@github.com:paceholder/nodeeditor.git
#make dir of cmake
mkdir build
cd build
#create unix file make
cmake ../nodeeditor -G"Unix Makefiles"; cd ../build;
#make lib
make
#back to root
cd ../
#build dips dir
mkdir dips/
mkdir dips/include
mkdir dips/linux/
mkdir dips/macOS/
#copy headers
find ./nodeeditor/src/ -name \*.hpp -exec cp {} dips/include/ \;
#copy lib (mac)
find ./build/lib/ -name \*.dylib -exec cp {} dips/macOS/ \;
#copy lib (linux)
find ./build/lib/ -name \*.so -exec cp {} dips/linux/ \;
#remove all
yes | rm -R build
yes | rm -R nodeeditor

#!/bin/sh
mkdir build
cd build
cmake -DKatydid_ENABLE_TESTING=TRUE -DCMAKE_BUILD_TYPE=STANDARD ..
make -j3 
make install
cd ..

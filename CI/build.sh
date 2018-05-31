#!/bin/sh
mkdir build
cd build
cmake -DKatydid_ENABLE_TESTING=TRUE ..
make -j3 
make install
cd ..

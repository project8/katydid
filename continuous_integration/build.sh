mkdir build
cd build
cmake ..
make -j3 
make install

export PATH=$(pwd)/bin:${PATH}
export INCLUDE_PATH=$(pwd)/include:${INCLUDE_PATH}
export LIBRARY_PATH=$(pwd)/lib:${LIBRARY_PATH}
export LD_LIBRARY_PATH=$(pwd)/lib:${LD_LIBRARY_PATH}
export LIBDIR=$(pwd)/lib:${LIBDIR}

cd ..
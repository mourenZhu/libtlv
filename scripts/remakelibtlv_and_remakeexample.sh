#!/bin/bash

echo "start remake libtlv"
cd ..
cd build
cmake ..
make clean
make
make install
cd ..

echo "start remake example"

cd example/simple_server_client/build
cmake ..
make clean
make

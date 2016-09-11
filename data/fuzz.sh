#!/usr/bin/env bash

SOURCE_DIR=`readlink -f $PWD/..`

echo $SOURCE_DIR

mkdir -p fuzz_build
pushd fuzz_build
    CC=afl-gcc CXX=afl-g++ cmake $SOURCE_DIR
    make -j12

    afl-fuzz -i $SOURCE_DIR/examples -o fuzz src/compiler @@
popd

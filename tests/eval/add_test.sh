#!/usr/bin/env bash

if [ -z $1 ]; then
    echo "Must supply a testname"
    exit -1
fi

TEST_NAME=$1
TEST_PATH=$(readlink -f $(pwd)/$TEST_NAME)

if [ -e $TEST_PATH ]; then
    echo "Test already exists"
    exit -1
fi

mkdir $TEST_PATH
pushd $TEST_PATH &> /dev/null
    touch main.x stderr.txt stdout.txt
    echo "0" > returncode.txt
popd &> /dev/null

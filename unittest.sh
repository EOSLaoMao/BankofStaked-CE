#!/bin/bash
RED='\033[01;31m'

cd tests

./build.sh

./build/tests/unit_test

if [ $? -eq 0 ]; then
    echo "Run unit test success"
else
    echo -e "-----------------------------------"
    echo -e "${RED}Please run ./build.sh first."
    echo -e "-----------------------------------"
fi

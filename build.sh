#!/bin/bash
IMAGE=eoslaomao/eos-dev:1.2.3
NAME=bankofstaked

docker ps | grep $NAME-eos-dev
if [ $? -ne 0 ]; then
    echo "Run eos dev env "
    docker run --name $NAME-eos-dev -dit --rm -v  `(pwd)`:/$NAME $IMAGE
fi

docker exec $NAME-eos-dev eosiocpp -g /$NAME/$NAME.abi /$NAME/src/$NAME.cpp
docker exec $NAME-eos-dev eosiocpp -o /$NAME/$NAME.wast /$NAME/src/$NAME.cpp
docker cp ../$NAME nodeosd:/
docker cp scripts nodeosd:/


if [ ! -d build ]; then
    mkdir build
    echo "Create build dir!!"
fi

mv $NAME.abi ./build
mv $NAME.wast ./build
mv $NAME.wasm ./build

echo "Build SUCCESS!!!"

./unittest.sh

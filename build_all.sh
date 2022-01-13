#!/usr/bin/env bash

git submodule update --init

# Get third party libraries
pushd mc/libs/simplex
git submodule update --init --recursive
GXX_FLAGS="-std=c++0x -fstack-protector-all -rdynamic -fstack-protector-all -Wall -Wextra -Wswitch-default -Wswitch-enum -Winit-self -Wformat-security -Wfloat-equal -Wcast-qual -Wconversion -Wlogical-op -Winline -Wfloat-conversion -Wshadow"
# -Wno-c++98-compat -Wno-c++98-compat-pedantic
CLANG_FLAGS="-Weverything -Wno-padded -Wno-covered-switch-default -Wno-unreachable-code -fPIE"
g++ -O2 src/SimplexNoise.cpp -c -o src/simplex.a $GXX_FLAGS
popd

pushd mc/libs/glad
echo $GXX_FLAGS
clang -O2 glad.c -c -o glad.a -I./ $CLANG_FLAGS
popd

# Build the texture packer
pushd mc/misc
./build.sh
popd
# Generate the texture atlas
pushd mc/build
./packer
popd

# Build the game
pushd mc/src
./build.sh
popd


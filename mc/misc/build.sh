#!/usr/bin/env bash

mkdir -p ../build

CFLAGS="-Wall -Wextra -fno-rtti -fno-exceptions -Wno-write-strings -Wno-missing-field-initializers -std=c++17"
CFLAGS="$CFLAGS -I../libs/stb -I../src"
g++ -O2 texture_packer.cpp -o ../build/packer $CFLAGS

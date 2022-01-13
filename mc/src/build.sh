#!/usr/bin/env bash

mkdir -p ../build

CFLAGS="-Wall -Wextra -fno-rtti -fno-exceptions -Wno-write-strings -Wno-missing-field-initializers -std=c++17"
CFLAGS="$CFLAGS -I../libs/glfw/linux/include -I../libs/stb -I../libs -I../libs/simplex/src -I../libs/glad/"
LDFLAGS="../build/mc.a ../libs/glad/glad.a ../libs/glfw/linux/libglfw3.a ../libs/simplex/src/simplex.a -lm -ldl -lpthread -lGL"

MODE="-g"
g++ $MODE mc.cpp -c -o ../build/mc.a $CFLAGS -fPIC
g++ $MODE glfw_mc.cpp -o ../build/game $CFLAGS $LDFLAGS
rm -f ../build/mc.a

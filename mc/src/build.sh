#!/usr/bin/env bash

mkdir -p ../build/web

# -lGLU g++ $MODE x11_mc.cpp -o ../build/game $CFLAGS $LDFLAGS -lX11 -lXi

INCLUDE="$CFLAGS -I../libs/glfw/linux/include -I../libs/stb -I../libs -I../libs/simplex/src -I../libs/glad/ -I../../libs -I../../libs/simplex/src"
CFLAGS="-Wall -Wextra -fno-rtti -fno-exceptions -Wno-write-strings -Wno-missing-field-initializers -std=c++11 $INCLUDE"
LDFLAGS="../build/mc.a ../libs/glad/glad.a ../libs/simplex/src/simplex.a ../libs/glfw/linux/libglfw3.a -lm -ldl -lpthread -lGL"

if [ "$1" == "run" ]; then
	pushd ../build/web
	python -m SimpleHTTPServer 5001
	popd
else
	MODE="-g"
	g++ $MODE mc.cpp -c -o ../build/mc.a $CFLAGS -fPIC
	g++ $MODE glfw_mc.cpp -o ../build/game $CFLAGS $LDFLAGS
	rm -f ../build/mc.a
	
	pushd ../build/web
	## Assets
	rm -rf assets/
	mkdir -p assets
	cp ../../assets/mc_atlas.png assets/
	cp ../../assets/uv_location.mc assets/
	## WASM -s INITIAL_MEMORY=<X> -s PTHREAD_POOL_SIZE=1 -s USE_PTHREADS=1
	WASM_FLAGS="-std=c++11 -Wall -Wextra -Wno-write-strings -O2"
	WASM_FLAGS="$WASM_FLAGS -s USE_WEBGL2=1 -s FULL_ES3=1 -s USE_GLFW=3 -s WASM=1 -s MAX_WEBGL_VERSION=2 -s MIN_WEBGL_VERSION=2 -s LLD_REPORT_UNDEFINED -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_RUNTIME_METHODS=['ccall']"
	emcc ../../src/web_mc.cpp ../../src/mc.cpp ../../libs/simplex/src/SimplexNoise.cpp -o index.js $INCLUDE $WASM_FLAGS --preload-file assets
	popd
fi

@echo off

REM NOTE: Haven't tested this yet

call mc\misc\shell.bat 

IF NOT EXIST mc\build mkdir mc\build

set CFLAGS=-nologo -fp:fast -EHa- -wd4201 -wd4100
set INC=-I.\

git submodule update --init

pushd mc\libs\glad
cl -O2 -c glad.c %CFLAGS% %INC%
lib glad.obj
del *.obj
popd

pushd mc\libs\simplex\src
git submodule update --init --recursive
cl -O2 -c SimplexNoise.cpp %CFLAGS% %INC%
lib SimplexNoise.obj
del *.obj
popd

call mc\src\build.bat

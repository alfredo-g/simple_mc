@echo off

call ..\misc\shell.bat 

IF NOT EXIST ..\build mkdir ..\build

set CFLAGS=-nologo -fp:fast -EHa- -wd4201 -wd4100
set INC=-I.\ -I..\libs\glfw\WIN64\include -I..\libs\stb -I..\libs -I..\libs\simplex\src -I..\libs\glad
set LDFLAGS=-incremental:no -opt:ref mc.lib ..\libs\glad\glad.lib ..\libs\glfw\WIN64\lib-vc2015\glfw3.lib ..\libs\simplex\src\SimplexNoise.lib user32.lib gdi32.lib winmm.lib opengl32.lib Shell32.lib msvcrt.lib

pushd ..\build
cl -O2 ..\src\mc.cpp -c %CFLAGS% %INC%
lib mc.obj
cl -O2 ..\src\glfw_mc.cpp /o game %CFLAG% %INC% /link %LDFLAGS%
del *.lib
del *.obj
popd

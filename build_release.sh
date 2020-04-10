#!/bin/bash

# I know it sucks, I just have no shame

# g++ src/*.cpp -O2 -march=core2 -msse2 -Wall -Wextra -Wno-deprecated-copy -Werror -o bin/panzerToy.exe -Iinclude -Llib/x64 -Llib -lgl3w -lglfw3 -lgdi32 -lassimp
clang src/*.cpp -O2 -march=core2 -msse2 -Wall -Wextra -Werror -o bin/panzerToy.exe -Iinclude -Llib/x64 -Llib -lgl3w -lglfw3 -lgdi32 -lassimp -lstdc++

# This N.E.E.D.S. to be done in CMake instead of a makeshift thingy like this

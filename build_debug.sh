#!/bin/bash

# I know it sucks, I just have no shame

# g++ -ggdb src/*.cpp -std=c++17 -msse2 -Wall -Wextra -Wno-deprecated-copy -o bin/panzerToy.exe -Iinclude -Llib/x64 -Llib -lgl3w -lglfw3 -lgdi32 -lassimp
clang src/*.cpp -ggdb -std=c++17 -msse2 -Wall -Wextra -o bin/panzerToy.exe -Iinclude -Llib/x64 -Llib -lgl3w -lglfw3 -lgdi32 -lassimp -lstdc++

# This N.E.E.D.S. to be done in CMake instead of a makeshift thingy like this

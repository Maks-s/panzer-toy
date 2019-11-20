#!/bin/bash

# I know it sucks, I just have no shame

g++ src/*.cpp -O2 -Wall -Wextra -Werror -o bin/panzerToy.exe -Iinclude -Llib/x64 -Llib -lgl3w -lglfw3 -lgdi32

# This N.E.E.D.S. to be done in CMake instead of a makeshift thingy like this

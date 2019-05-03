#!/bin/sh
#-fsanitize=address
g++  main.cpp    -w -std=c++11 -g -fsanitize=address -g 

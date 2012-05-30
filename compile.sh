#!/bin/bash
g++ -g -c -o main.o main.cpp &&
g++ -g -c -o interface.o interface.cpp &&
g++ -g -c -o parser.o parser.cpp &&
g++ -Wl -o calculate main.o parser.o interface.o

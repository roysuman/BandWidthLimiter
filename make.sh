#!/bin/bash
cd BandwidthLimiter
make -f Makefile || exit
gcc -g resilience.c -lm -o ../resilience -Wall -Wextra -Wno-write-strings -pthread

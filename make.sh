#!/bin/bash
cd BandwidthLimiter
make -f Makefile || exit
gcc -g resilience.c -lm -o ../blimiter -Wall -Wextra -Wno-write-strings -pthread

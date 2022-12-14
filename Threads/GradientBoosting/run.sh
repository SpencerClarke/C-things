#!/bin/sh

gcc dt.c -lm -fopenmp
./a.out | python3 plot.py

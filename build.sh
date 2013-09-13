#!/bin/sh -xe

gcc -Wall -std=c99 -o gdb-with-breakpoints gdb-with-breakpoints.c -lssl -lbfd -ldl -lz -liberty
gcc -Wall -g -o example example.c

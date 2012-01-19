#!/bin/sh -xe

gcc -Wall -std=c99 -lbfd -o gdb-with-breakpoints gdb-with-breakpoints.c
gcc -Wall -g -o example example.c

#!/bin/bash

gcc -ansi -I include -pedantic-errors -Wall -Wextra -g source/calc.c test/calc_test.c -fPIC -lstack -L. -Wl,-rpath="\$ORIGIN" -lm

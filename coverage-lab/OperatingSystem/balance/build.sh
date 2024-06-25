#!/bin/bash

set -x
# gcc -o balance balance.c
clang -O0 -g -fsanitize=address -fsanitize-coverage=trace-pc-guard balance.c -o balance
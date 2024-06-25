#!/bin/bash

set -x
./autogen.sh
CC='clang' CFLAGS="-O0 -g -fsanitize=address -fsanitize-coverage=edge,trace-pc-guard" ./configure --enable-static --disable-shared
make -j4

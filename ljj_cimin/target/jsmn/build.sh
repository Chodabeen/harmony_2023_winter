#!/bin/bash

set -x

export ASAN_OPTIONS=detect_leaks=0:halt_on_error=1
CC="afl-gcc" CFLAGS="-O0 -g -fsanitize=address -static" LDFLAGS="-fsanitize=address" make jsondump


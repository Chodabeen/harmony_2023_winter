#!/bin/bash

set -x

# export ASAN_OPTIONS=detect_leaks=0:halt_on_error=1
export ASAN_OPTIONS=detect_leaks=0:halt_on_error=1
# CC="clang" CFLAGS="-O0 -g -fsanitize=address" LDFLAGS="-fsanitize=address" make jsondump
# CC="clang" CFLAGS="-O0 -g -fprofile-instr-generate -fcoverage-mapping -fsanitize=address -fsanitize-coverage=trace-pc-guard" LDFLAGS="-O0 -g -fprofile-instr-generate -fcoverage-mapping -fsanitize=address -fsanitize-coverage=trace-pc-guard" make jsondump
CC="clang" CFLAGS="-O0 -g -fsanitize=address -fsanitize-coverage=trace-pc-guard" LDFLAGS="-fsanitize=address -fsanitize-coverage=trace-pc-guard" make jsondump
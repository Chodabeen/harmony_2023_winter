#!/bin/bash

set -x

export CFLAGS="-O2 -fno-omit-frame-pointer -gline-:qtables-only -fsanitize=address,fuzzer-no-link -fsanitize-address-use-after-scope"
export CC="clang-12"

build_lib(){
	rm -rf myhtml
	git clone https://github.com/lexborisov/myhtml.git
	(cd myhtml && make static)
}
build_fuzzer(){
	clang-12 -g -Imyhtml/include ${CFLAGS} -fsanitize=fuzzer target.cc myhtml/lib/libmyhtml_static.a -o fuzzer_myhtml
}

#build_lib
build_fuzzer

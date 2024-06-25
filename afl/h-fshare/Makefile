all: fshare fshared

fshare: fshare.c
	gcc -g -o fshare fshare.c 

fshared: fshared.c
	gcc -g -pthread -o fshared fshared.c

clean:
	rm -rf fshare fshared




gtest: gtest.c
	gcc gtest.c -o gtest -Dgtest
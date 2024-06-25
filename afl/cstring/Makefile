CC=../AFLplusplus/afl-clang-fast

# CFLAGS="-O0 -g -fsanitize=address"
# LDFLAGS="-fsanitize=address"

test : test.c cstring.c 
	$(CC) -g -Wall -O0 --coverage -o $@ $^
	
gcov :
	gcov -bcif cstring.gcda

afl:
	../AFLplusplus/afl-fuzz -i inputs -o outputs ./test

fuzz:
	clang -g -O0 -fsanitize=fuzzer,address -o fuzz fuzz.c cstring.c
# ./fuzz -only_ascii=1

clean:
	rm -rf *.gcno *.gcda *.gcov test fuzz

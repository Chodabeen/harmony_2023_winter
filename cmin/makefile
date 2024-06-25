CC=gcc
CFLAGS=-g -Wall

all: cmin

cmin: cmin.c
	$(CC) $(CFLAGS) cmin.c -o cmin

jsmn:
	./cmin -i ./OperatingSystem/jsmn/testcases/crash.json -m "AddressSanitizer: heap-buffer-overflow" -o reduced ./OperatingSystem/jsmn/jsondump

balance:
	./cmin -i ./OperatingSystem/balance/testcases/fail -m "AddressSanitizer: heap-buffer-overflow" -o reduced ./OperatingSystem/balance/balance

libxml2:
	./cmin -i ./OperatingSystem/libxml2/testcases/crash.xml -m "SEGV on unknown address" -o reduced ./OperatingSystem/libxml2/xmllint " --recover --postvalid -"

libpng:
	cp OperatingSystem/libpng/crash.png .
	cp OperatingSystem/libpng/libpng . -r
	./cmin -i crash.png -m "MemorySanitizer: use-of-uninitialized-value" -o reduced ./libpng/test_pngfix

clean:
	rm -f cmin reduced

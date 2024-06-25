#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cstring.h"

int FuzzMe(const uint8_t * Data, size_t DataSize) {
    if (DataSize < 2 || Data[DataSize - 1] != '\0' || DataSize > 1023) {
        return 1;
    }
    // char str1[1024];
    // strcpy(str1, (char *)Data);
    char * str1 = (char *)Data;
    char str2[1024];
    // memcpy(str2, str1, DataSize);
    // strcpy(str2, str1);

    // test
    CSTRING_BUFFER(a);
    cstring_printf(a, "%s", str1);
    // foo
    CSTRING_LITERAL(hello, str1);
    CSTRING_BUFFER(ret);
    if (cstring_equal(hello, CSTRING(a))) {
		cstring_cat(ret, "equal");
	} else {
		cstring_cat(ret, "not equal");
        fprintf(stderr, "str1: %s\nstr2: %s\n", str1, str2);
        return 0;
        // CSTRING_CLOSE(a);
        // return 0;
	}
    cstring b = cstring_grab(CSTRING(ret));

    printf("%s\n", b->cstr);
	// cstring_printf(a, "very long string %01024d",0);
	// printf("%s\n", CSTRING(a)->cstr);
    CSTRING_CLOSE(a);
    cstring_release(b);
    
 
    return 0;
}


int LLVMFuzzerTestOneInput(const uint8_t * Data, size_t Size) {
    FuzzMe(Data, Size);
    return 0;
}

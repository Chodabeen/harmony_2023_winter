#include "cstring.h"
#include <stdio.h>

static cstring
foo(cstring t, char * str) {
	CSTRING_LITERAL(hello, str);
	CSTRING_BUFFER(ret);
	if (cstring_equal(hello,t)) {
		cstring_cat(ret, "equal");
	} else {
		cstring_cat(ret, "not equal");
	}
	return cstring_grab(CSTRING(ret));
}

static void
test(char * str1, char * str2) {
	CSTRING_BUFFER(a);
	cstring_printf(a, "%s", str1);
	cstring b = foo(CSTRING(a), str2);
	printf("%s\n", b->cstr);
	cstring_printf(a, "very long string %01024d",0);
	printf("%s\n", CSTRING(a)->cstr);
	CSTRING_CLOSE(a);
	cstring_release(b);
}

int
main() {
	char str1[1024];
	char str2[1024];
	scanf("%s", str1);
	strcpy(str2, str1);
	test(str1, str2);
	return 0;
}


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

extern "C" int LLVMFuzzerTestOneInput (const uint8_t *data, size_t size);

/* fuzz target entry point, works without libFuzzer */

int main(int argc, char **argv)
{
    FILE *f;
    char *buf = NULL;
    long siz_buf;

    if(argc < 2)
    {
        fprintf(stderr, "no input file\n");
        goto err;
    }

    f = fopen(argv[1], "rb");
    if(f == NULL)
    {
        fprintf(stderr, "error opening input file %s\n", argv[1]);
        goto err;
    }

    fseek(f, 0, SEEK_END);

    siz_buf = ftell(f);
    rewind(f);

    if(siz_buf < 1) goto err;

    buf = (char*)malloc((size_t)siz_buf);
    if(buf == NULL)
    {
        fprintf(stderr, "malloc() failed\n");
        goto err;
    }

    if(fread(buf, (size_t)siz_buf, 1, f) != 1)
    {
        fprintf(stderr, "fread() failed\n");
        goto err;
    }

    (void)LLVMFuzzerTestOneInput((uint8_t*)buf, (size_t)siz_buf);

err:
    free(buf);

    return 0;
}

bool FuzzMe(const uint8_t *Data, size_t DataSize) {
  return DataSize >= 3 &&
      Data[0] == 'A' &&
      Data[1] == 'B' &&
      Data[2] == 'C' &&
      Data[3] == 'Z';
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FuzzMe(Data, Size);
return 0;
}
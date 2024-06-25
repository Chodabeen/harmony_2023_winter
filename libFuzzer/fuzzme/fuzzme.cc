#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

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
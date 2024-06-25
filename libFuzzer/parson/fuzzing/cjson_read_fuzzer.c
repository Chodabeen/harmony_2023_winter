#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "../parson.h"

// json_parse_file call [read_file, ]
// json_parse_file_with_comments call [read_file, ]

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size); /* required by C89 */

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    // JSON_Value * output_value = NULL;
    // if ((output_value = json_parse_string((char *)data)) != NULL) {
    //     printf("normal data\n");
    // }

    if (json_parse_file("inputs/test1") != NULL) {
        fprintf(stderr, "normal data\n");
    }


    return 0;
}

#ifdef __cplusplus
}
#endif
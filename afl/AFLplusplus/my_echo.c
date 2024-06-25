#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

void my_echo (char * data) {
    char buf[10];
    strcpy(buf, data);
    printf("%s\n", buf);
    if (data[0] == 'f') {
        if (data[1] == 'u') {
            if (data[2] == 'z') {
                if (data[3] == 'z') {
                    assert(0);
                }
            }
        }
    }
}

int main() {
    char inp[50];
    read(STDIN_FILENO, inp, 50);
    my_echo(inp);
}
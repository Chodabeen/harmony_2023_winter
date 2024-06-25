#include <stdio.h>

// weak 속성을 가진 함수 선언
void weak_function(void) __attribute__((weak));

int main() {
    
    if (weak_function) {
        weak_function();
    } else {
        printf("weak_function is not defined.\n");
    }

    return 0;
}

#include <stdio.h>
#include <vreg.hpp>
int main(int argc, char **argv) {
    (void)argc, (void)argv;
    
    printf("size=%ld\n", sizeof(vreg::VRegConst<int*[1]>));
    printf("size=%ld\n", sizeof(vreg::VRegConst<int*[2]>));
    return 0;
}
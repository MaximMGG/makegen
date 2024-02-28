#include "../headers/generator.h"
#include <stdio.h>


int main() {

    Generator_init();
    Generator_get_sources("/home/maxim/cprojects/makegen/");


    for(int i = 0; i < src->len; i++) {
        printf("%s\n", (char *)list_get(src, i));
    }

    Generator_free();

    return 0;
}

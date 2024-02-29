#include "../headers/generator.h"
#include <stdio.h>
#include <util/m_list.h>

int main(int argc, char **argv) {
    List *arg_list = list_create_from_array((void **)argv, M_STRING, argc);
    Generator_init();
    Generator_get_sources("./");
    Generator_setMakeflag(1, NULL);
    Generator_generate();
    Generator_free();

    return 0;
}

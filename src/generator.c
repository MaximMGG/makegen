#include "../headers/generator.h"
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <util/m_list.h>

static char generator_type;
static char generator_leng;

static char *make_content;
List *src;

struct Make_flags_s{
    char compiler[5];
    List *warnings;
    char *libs;
    char *debug;
} Make_flags;


void Generator_init() {
    src = list_create(0, M_STRING);
}

void Generator_free() {
    list_free_all(src);
    free(make_content);
}

//TODO(Maxim) need to continue setting flags
void Generator_setMakeflag(int argc, char **argv) { 
    for(int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch(argv[i][1]) {
                case 'c': {
                    strncpy(Make_flags.compiler, argv[i + 1], 4);
                } break;
                case 'w': {
                    Make_flags.warnings = list_create(0, M_STRING);
                    for(int j = i + 1; j < argc || argv[j][0] != '-'; j++) {
                        list_add(Make_flags.warnings, argv[j]);
                    }
                }
            }
        }
    }
}


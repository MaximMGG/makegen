#include "../headers/generator.h"
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <util/m_list.h>

static char *make_content = NULL;
List *src = NULL;

struct Make_flags_s{
    char compiler[5];
    List *warnings;
    List *libs;
    char debug[3];
    int generator_type;
} Make_flags;


void Generator_init() {
    src = list_create(0, M_STRING);
}

void Generator_free() {
    if (Make_flags.warnings != NULL) {
        list_free_all(Make_flags.warnings);
    }
    if (Make_flags.libs != NULL) {
        list_free_all(Make_flags.libs);
    }
    
    if (src != NULL) {
        list_free_all(src);
    }
    if (make_content != NULL) {
        free(make_content);
    }
}

//TODO(Maxim) need to continue setting flags
void Generator_setMakeflag(int argc, char **argv) { 
    Make_flags.generator_type = 0;
    Make_flags.warnings = NULL;
    Make_flags.compiler[0] = 0;
    Make_flags.libs = NULL;
    Make_flags.debug[0] = 0;
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
                case 'g': {
                    if (strcmp(argv[i], "all") == 0) {
                        Make_flags.generator_type = GENERATE_WHOLLE_APP;
                    } else if (strcmp(argv[i], "cur") == 0) {
                        Make_flags.generator_type = GENERATE_CUR_DIR;
                    }
                }
                case 'd': {
                    strncpy(Make_flags.debug, "-g", 3);
                }
                case 'l': {
                    Make_flags.warnings = list_create(0, M_STRING);
                    for(int j = i + 1; j < argc || argv[j][0] != '-'; j++) {
                        list_add(Make_flags.libs, argv[j]);
                    }
                }
            }
        }
    }
    if (Make_flags.generator_type == 0) Make_flags.generator_type = GENERATE_WHOLLE_APP;
    if (Make_flags.compiler[0] == 0) strncpy(Make_flags.compiler, "gcc", 5);
}

void Generator_get_sources(char *path) {
    DIR *dir = opendir(path);
    struct dirent *d;
    struct stat s;
    char buf[200];
    memset(buf, 0, 200);
    strcpy(buf, path);
    int offset = strlen(path);

    while((d = readdir(dir))) {
        strcpy(buf + offset, d->d_name);

        if (stat(buf, &s) != -1) {
            if (S_ISREG(s.st_mode)) {
                if(s.st_mode & S_IXUSR) {

                } else {
                    str *temp = newstr(buf);
                    if (str_end_with(temp, ".c"))
                        list_add(src, buf);
                    str_free(temp);
                }
            } else if (S_ISDIR(s.st_mode) && Make_flags.generator_type != GENERATE_CUR_DIR) {
                str *temp = newstr(buf);
                if (str_end_with(temp, ".") || str_end_with(temp, "..") || str_end_with(temp, ".git")) {
                } else {
                    str *f = newstr("/");
                    str_concat(temp, f);
                    Generator_get_sources(temp->str);
                    str_free(f);
                }
                str_free(temp);
            }
        }
    }
}


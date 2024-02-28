#include "../headers/generator.h"
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
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
            } else if (S_ISDIR(s.st_mode)) {
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

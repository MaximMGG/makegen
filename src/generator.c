#include "../headers/generator.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <util/m_list.h>

static char *make_content = NULL;
List *src = NULL;

struct Make_flags_s{
    str *binary_name;
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
    if (Make_flags.binary_name != NULL) {
        str_free(Make_flags.binary_name);
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
                } break;
                case 'g': {
                    if (strcmp(argv[i], "all") == 0) {
                        Make_flags.generator_type = GENERATE_WHOLLE_APP;
                    } else if (strcmp(argv[i + 1], "cur") == 0) {
                        Make_flags.generator_type = GENERATE_CUR_DIR;
                    }
                } break;
                case 'd': {
                    strncpy(Make_flags.debug, "-g", 3);
                } break;
                case 'l': {
                    Make_flags.warnings = list_create(0, M_STRING);
                    for(int j = i + 1; j < argc || argv[j][0] != '-'; j++) {
                        list_add(Make_flags.libs, argv[j]);
                    }
                } break;
                case 'n': {
                    Make_flags.binary_name = newstr(argv[i + 1]); 
                } break;
            }
        }
    }
    if (Make_flags.generator_type == 0) Make_flags.generator_type = GENERATE_WHOLLE_APP;
    if (Make_flags.compiler[0] == 0) strncpy(Make_flags.compiler, "gcc", 5);
    if (Make_flags.binary_name == NULL) Make_flags.binary_name = newstr("a.out");
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

static str *source_str() {
    if (src == NULL || src->len == 0) {
        return NULL;
    }
    str *source =  newstr(list_get(src, 0));
    for(int i = 1; i < src->len; i++) {
        source = str_append(source, " ");
        source = str_append(source, list_get(src, i));
    }
    return source;
};

static str *libs_str() {
    if (Make_flags.libs == NULL) {
        return NULL; 
    }
    str *libs = newstr("");
    for(int i = 0; i < Make_flags.libs->len; i++) {
        libs = str_append(libs, " -l");
        libs = str_append(libs, list_get(Make_flags.libs, i));
    }

    return libs;
}

static str *warnings_str() {
    if (Make_flags.warnings == NULL) {
        return NULL; 
    }
    str *warnings = newstr("");
    for(int i = 0; i < Make_flags.libs->len; i++) {
        warnings = str_append(warnings, " -W");
        warnings = str_append(warnings, list_get(Make_flags.warnings, i));
    }
    return warnings;
}

void Generator_generate() {
    str *source = source_str();
    if (source == NULL) {
        fprintf(stderr, "Do not found source files\n");
        exit(1);
    }
    str *libs = libs_str();
    str *warnings = warnings_str();
    int total_len = source->len;
    total_len += libs == NULL ? 0 : libs->len;
    total_len += warnings == NULL ? 0 : warnings->len;
    total_len += strlen(MAKE_MSG);
    total_len += Make_flags.binary_name->len;
    total_len += 10;

    char buf[total_len];

    snprintf(buf, total_len, MAKE_MSG, 
                Make_flags.binary_name->str, 
                Make_flags.compiler, 
                Make_flags.debug[0] == 0 ? "": "-g", 
                warnings == NULL ? "" : warnings->str, 
                libs == NULL ? "" : libs->str,
                source->str);

    FILE *f = fopen("Makefile", "w");

    fputs(buf, f);

    fclose(f);
    str_free(libs);
    str_free(warnings);
    str_free(source);


}

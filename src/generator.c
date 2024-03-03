#include "../headers/generator.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <util/m_list.h>

#define GEN_ERROR(msg) fprintf(stderr, "func - %s, line -%d, msg - %s\n", __FUNCTION__, __LINE__, msg)

static char *make_content = NULL;
List *src = NULL;

static struct Make_flags_s Make_flags;

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

static boolean str_is_flag(char *s) {

    int len = strlen(s);
    if (len == 2 && s[0] == '-') return true;

    return false;
}

//TODO(Maxim) need to continue setting flags
void Generator_setMakeflag(List *args) { 
    Make_flags.generator_type = 0;
    Make_flags.warnings = NULL;
    Make_flags.compiler[0] = 0;
    Make_flags.libs = NULL;
    Make_flags.debug[0] = 0;
    for(int i = 1; i < args->len; i++) {
        char *temp = list_get(args, i);

        if (strcmp(temp, "-c") == 0) {
            strncpy(Make_flags.compiler, list_get(args, i + 1), 4);
            i++;
        } else if (strcmp(temp, "-w") == 0) {
            Make_flags.warnings = list_create(0, M_STRING);
            temp = list_get(args, i + 1);
            while(!str_is_flag(temp)) {
                list_add(Make_flags.warnings, temp);
                i++;
                temp = list_get(args, i + 1);
                if (temp == NULL) {
                    break;
                }
            }
        } else if (strcmp(temp, "-g") == 0) {
            char *b = list_get(args, i + 1);
            if (strcmp(b, "all")) {
                Make_flags.generator_type = GENERATE_WHOLLE_APP;
            } else if (strcmp(b, "cur")) {
                Make_flags.generator_type = GENERATE_CUR_DIR;
            } else {
                Make_flags.generator_type = GENERATE_WHOLLE_APP;
            }
        } else if (strcmp(temp, "-d") == 0) {
            strcpy(Make_flags.debug, "-g");
        } else if (strcmp(temp, "-l") == 0) {
            temp = list_get(args, i + 1);
            Make_flags.libs = list_create(0, M_STRING);
            while(!str_is_flag(temp)) {
                list_add(Make_flags.libs, temp);
                i++;
                temp = list_get(args, i + 1);
                if (temp == NULL) {
                    break;
                }
            }
        } else if (strcmp(temp, "-n") == 0) {
            Make_flags.binary_name = newstr(list_get(args, i + 1));
        } else if (strcmp(temp, "-us") == 0) {
            Generator_update_sources();
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

static boolean old_str_starts_with(char *d, char *pattern) {

    int plen = strlen(pattern);

    for(int i = 0; i < plen; i++) {
        if (d[i] != pattern[i]) {
            return false;
        }
    }
    return true;
}

void Generator_update_sources() {
    Generator_get_sources(DEFAULT_PATH);

    char buf_source[512];
    str *src_str = source_str();
    snprintf(buf_source, 512, G_SRC, src_str->str);
    str_free(src_str);

    FILE *mf = fopen("Makefile", "r+");
    List *fcon = list_create(0, M_STRING);

    if (mf == NULL) {
        GEN_ERROR("Makefile doesn't exist");
        Generator_free();
        exit(EXIT_FAILURE);
    }

    char buf[512];
    memset(buf, 0, 512);

    while(!feof(mf)) {
        fgets(buf, 512, mf);
        if (old_str_starts_with(buf, "SRC")) {
            list_add(fcon, buf_source);
            continue;
        }
        list_add(fcon, buf);
    }

    fseek(mf, 0, SEEK_SET);

    for(int i = 0; i < fcon->len; i++) {
        fputs(list_get(fcon, i), mf);
    }

    fclose(mf);
    list_free_all(fcon);

    Generator_free();
    exit(EXIT_SUCCESS);
}

void Generator_generate() {
    str *source = source_str();
    if (source == NULL) {
        fprintf(stderr, "Do not found source files\n");
        exit(1);
    }
    char buf[2000];
    str *libs = libs_str();
    str *warnings = warnings_str();
    FILE *f = fopen("Makefile", "w");
    str *msg = newstr("");

    snprintf(buf, 2000, G_BINARY, Make_flags.binary_name->str);
    msg = str_append(msg, buf);
    snprintf(buf, 2000, G_CC, Make_flags.compiler);
    msg = str_append(msg, buf);
    snprintf(buf, 2000, G_FLAGS, warnings == NULL ? "" : warnings->str);
    msg = str_append(msg, buf);
    snprintf(buf, 2000, G_DEBUG, Make_flags.debug);
    msg = str_append(msg, buf);
    snprintf(buf, 2000, G_LIBS, libs == NULL ? "" : libs->str);
    msg = str_append(msg, buf);
    snprintf(buf, 2000, G_SRC, source->str);
    msg = str_append(msg, buf);
    msg = str_append(msg, MAKE_MSG);

    fputs(msg->str, f);

    fclose(f);
    str_free(libs);
    str_free(warnings);
    str_free(source);
    str_free(msg);
}

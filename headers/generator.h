#ifndef _GENERATIR_H_
#define _GENERATIR_H_

#include <util/m_list.h>
#include <util/m_string.h>

#define GENERATE_C 1
#define GENERATE_CPP 2

#define GENERATE_WHOLLE_APP 3
#define GENERATE_CUR_DIR 4

#define G_BINARY "BINARY = %s\n"
#define G_CC "CC = %s\n"
#define G_FLAGS "FLAGS = %s\n"
#define G_DEBUG "DEBUG = %s\n"
#define G_LIBS "LIBS = %s\n"
#define G_SRC "SRC = %s\n"

#define MAKE_MSG    "OBJ = $(patsubst %.c, %.o, $(SRC))\n\n"                    \
                    "all: $(BINARY)\n\n"                                        \
                    "$(BINARY): $(OBJ)\n"                                       \
                    "\t$(CC) -o $(BINARY) $^ $(FLAGS) $(LIBS) $(DEBUG)\n"    \
                    "%.o: %.c\n"                                                \
                    "\t$(CC) -o $@ -c $< $(FLAGS) $(LIBS) $(DEBUG)\n"        \
                    "clean: \n"                                                 \
                    "\trm $(BINARY) $(OBJ)\n"                                \
                    "#r"


extern List *src;

struct Make_flags_s{
    str *binary_name;
    char compiler[5];
    List *warnings;
    List *libs;
    char debug[3];
    int generator_type;
};

void Generator_init();
void Generator_free();
void Generator_setMakeflag(List *args);
void Generator_get_sources(char *path);
void Generator_generate();

#endif //_GENERATIR_H_

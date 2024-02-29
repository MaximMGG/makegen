#ifndef _GENERATIR_H_
#define _GENERATIR_H_

#include <util/m_list.h>
#include <util/m_string.h>

#define GENERATE_C 1
#define GENERATE_CPP 2

#define GENERATE_WHOLLE_APP 3
#define GENERATE_CUR_DIR 4


#define MAKE_MSG    "BINARY = %s\n"                                             \
                    "CC = %s\n"                                                 \
                    "FLAGS = %s\n"                                              \
                    "DEBUG = %s\n"                                              \
                    "LIBS = %s\n"                                               \
                    "SRC = %s\n"                                                \
                    "OBJ = $(patsubst %.c, %.o, $(SRC))\n\n"                    \
                    "all: $(BINARY)\n\n"                                        \
                    "$(BINARY): $(OBJ)\n"                                       \
                    "   $(CC) $(FLAGS) -o $(BINARY) $^ $(DEBUG) $(LIBS)\n\n"    \
                    "%.o: %.c\n"                                                \
                    "   $(CC) -o $@ -c $< $(DEBUG)\n\n"                         \
                    "clean: \n"                                                 \
                    "   rm $(BINARY) $(OBJ)\n\n"                                   


extern List *src;

void Generator_init();
void Generator_free();
void Generator_setMakeflag(int argc, char **argv);
void Generator_get_sources(char *path);
void Generator_generate();

#endif //_GENERATIR_H_

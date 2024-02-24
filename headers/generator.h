#ifndef _GENERATIR_H_
#define _GENERATIR_H_

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


void Generator_init();


#endif //_GENERATIR_H_
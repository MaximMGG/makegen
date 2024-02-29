BINARY = a.out
CC = gcc
FLAGS = 
DEBUG = 
LIBS = 
SRC = ./src/generator.c ./src/test.c
OBJ = $(patsubst %.c, %.o, $(SRC))

all: $(BINARY)

$(BINARY): $(OBJ)
   $(CC) $(FLAGS) -o $(BINARY) $^ $(DEBUG) $(LIBS)

%.o: %.c
   $(CC) -o $@ -c $< $(DEBUG)

clean: 
   rm $(BINARY) $(OBJ)


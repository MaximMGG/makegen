CC = gcc
BINARY = makegen
FLAGS = -g
LIBS = -l_util

SRC = src/generator.c src/test.c
OBJ = $(patsubst %.c, %.o, $(SRC))


$(BINARY): $(OBJ)
	$(CC) -o $(BINARY) $^ $(FLAGS) $(LIBS)

%.o: %.c
	$(CC) -o $@ -c $< $(FLAGS) $(LIBS)

clean:
	rm $(BINARY) $(OBJ)

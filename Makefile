BINARY = makegen
CC = gcc
FLAGS = 
DEBUG = -g 
LIBS =  -l_util
SRC = ./src/genapp.c ./src/generator.c
OBJ = $(patsubst %.c, %.o, $(SRC))

all: $(BINARY)

$(BINARY): $(OBJ)
	$(CC) -o $(BINARY) $^ $(FLAGS) $(LIBS) $(DEBUG)
%.o: %.c
	$(CC) -o $@ -c $< $(FLAGS) $(LIBS) $(DEBUG)
clean: 
	rm $(BINARY) $(OBJ)

deploy:
	sudo cp $(BINARY) /usr/sbin/

CC=gcc
FLAGS=-Wall -Wextra -std=c99
TARGETS=main.c settings.c memory_management.c file_operations.c helpers.c definitions.h
NAME=image_editor

build:
	$(CC) $(FLAGS) $(TARGETS) -o $(NAME) -lm

clean:
	rm $(NAME)

pack:
	zip -FSr 313CA_GheorgheAndreiBogdan_Tema3.zip README Makefile *.c *h

.PHONY: pack clean

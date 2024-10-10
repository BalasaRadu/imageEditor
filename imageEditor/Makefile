# Copyright 2023 Echipa PCLP1

# compiler setup
CC=gcc -std=c99
CFLAGS=-Wall -Wextra -std=c99

# define targets
TARGETS = image_editor

build: $(TARGETS)

image_editor: *.c
	$(CC) $(CFLAGS) main.c image.c -o image_editor -lm

clean:
	rm -f $(TARGETS) $(OBJS)

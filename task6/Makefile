EXECUTABLES = abonent_directory
CC = gcc
CFLAGS = -Wall -Wextra -Werror
HEADER = DM_abonent_directory.h

all: $(EXECUTABLES)

abonent_directory: main.c DM_abonent_directory.c
	$(CC) $(CFLAGS) -o $@ $^ -I ./

clean:
	rm -f $(EXECUTABLES)

SOURCES = array_revers.c half_positive_matrix.c simple_matrix.c snail_matrix.c
EXECUTABLES = array_revers half_positive_matrix simple_matrix snail_matrix

CC = gcc
CFLAGS = -Wall -Wextra -Werror

all: $(EXECUTABLES)

array_revers: array_revers.c
	$(CC) $(CFLAGS) -o $@ $<

half_positive_matrix: half_positive_matrix.c
	$(CC) $(CFLAGS) -o $@ $<

simple_matrix: simple_matrix.c
	$(CC) $(CFLAGS) -o $@ $<

snail_matrix: snail_matrix.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(EXECUTABLES)

EXECUTABLES = array_and_pointer byte_change change_result substr

CC = gcc
CFLAGS = -Wall -Wextra -Werror

all: $(EXECUTABLES)

array_and_pointer: array_and_pointer.c
	$(CC) $(CFLAGS) -o $@ $<

byte_change: byte_change.c
	$(CC) $(CFLAGS) -o $@ $<

change_result: change_result.c
	$(CC) $(CFLAGS) -o $@ $<

substr: substr.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(EXECUTABLES)

all: server client

server: server.c
	x86_64-w64-mingw32-gcc server.c -o server.exe -lws2_32

client: client.c
	gcc -Wall -Werror -o client client.c

clean:
	rm client server.exe
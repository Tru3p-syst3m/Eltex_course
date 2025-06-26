all: server client_udp client_tcp

server: server.c
	gcc -Wall -Werror -o server server.c

client_udp: client_udp.c
	gcc -Wall -Werror -o client_udp client_udp.c

client_tcp: client_tcp.c
	gcc -Wall -Werror -o client_tcp client_tcp.c

clean:
	rm client_tcp client_udp server
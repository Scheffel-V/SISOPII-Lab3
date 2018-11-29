all:
	gcc server_tcp.c -o server_tcp -lpthread
	gcc client_tcp.c -o client_tcp -lpthread
server_tcp: server_tcp.c
	gcc server_tcp.c -o server_tcp -lpthread
client_tcp: client_tcp.c
	gcc client_tcp.c -o client_tcp -lpthread
clean:
	rm -f server_tcp

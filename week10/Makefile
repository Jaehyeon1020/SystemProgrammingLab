all : client_2019314891 server_2019314891

client_2019314891 : client.c
	gcc -o client_2019314891 client.c

server_2019314891 : server.c
	gcc -o server_2019314891 server.c

server.o :
	gcc -c server.c

clean :
	rm client_2019314891 server_2019314891
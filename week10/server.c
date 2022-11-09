#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#define MAXLINE 80

int main (int argc, char *argv[]) {
	int listenfd, connfd, caddrlen;
	struct hostent *h;
	struct sockaddr_in saddr, caddr;
	char buf[MAXLINE], buf_data[MAXLINE];
	int port = atoi(argv[1]);

	/* Create listen socket */
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket() failed.\n");
		exit(1);
	}
	memset((char*)&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(port);

	/* Bind sockaddr (IP, etc.) to listen socket */
	if (bind(listenfd, (struct sockaddr*)&saddr, sizeof(saddr)) < 0) {
		printf("bind() failed\n");
		exit(2);
	}

	/* Listen to listen socket */
	if (listen(listenfd, 5) < 0) {
		printf("listen() failed\n");
		exit(3);
	}

	/* Accept connection request from clients */
	caddrlen = sizeof(caddr);

	if ((connfd = accept(listenfd, (struct sockaddr*)&caddr, (socklen_t*)&caddrlen)) < 0) {
		printf("accept() failed\n");
		exit(4);
	}

	printf("Server: accept() success.\n");

	while (1) {
		int n = 0, fd, total = 0;
		char *ptr;

		memset(buf, 0, MAXLINE);
		if((n = read(connfd, buf, MAXLINE)) > 0) {
			buf[n - 1] = '\0';
			if(!strcmp(buf, "quit")) {
				printf("Server: quit.\n");
				break;
			}

			printf("Server: receive filename %s.\n", buf);

			ptr = strstr(buf, "client");
			strncpy(ptr, "server", 6);
			fd = open(buf, O_WRONLY | O_CREAT, 0644);
			if (fd < 0) {
				printf("Server: file open() failed.\n");
			} else {
				memset(buf_data, 0, MAXLINE);
				if((n = read(connfd, buf_data, MAXLINE)) > 0) {
					write(fd, buf_data, n);
					printf("Server: receive %s file (%d bytes) from client.\n", buf, n);
				}

				close(fd);
			}
		}
	}

	close(connfd);

	return 0;
}


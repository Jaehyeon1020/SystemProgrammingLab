#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <fcntl.h>
#define MAXLINE 80

int main (int argc, char *argv[]) {
	struct hostent *h;
	struct sockaddr_in saddr;
	char buf[MAXLINE], buf_data[MAXLINE];
	int connfd;
	char *hostname = argv[1];
	int port = atoi(argv[2]);

	/* Create socket */
	if ((connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket() failed.\n");
		exit(1);
	}

	/* Get host information */
	if ((h = gethostbyname(hostname)) == NULL) {
		printf("invalid host name %s\n", hostname);
		exit(2);
	}
	memset((char*)&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	memcpy((char*)&saddr.sin_addr.s_addr, (char*)h->h_addr, h->h_length);
	saddr.sin_port = htons(port);

	/* Connect to host */
	if(connect(connfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
		printf("connect() failed.\n");
		exit(3);
	}
	
	printf("Client: connect() success.\n");

	while (1) {
		int n = 0, fd, total = 0;
		
		memset(buf, 0, MAXLINE);
		if ((n = read(0, buf, MAXLINE)) > 0) {
			buf[n - 1] = '\0';
			if (!strcmp(buf, "quit")) {
				printf("Client: quit.\n");
				write(connfd, buf, n);
				break;
			}

			fd = open(buf, O_RDONLY);
			if (fd < 0) {
				printf("Client: file open() failed.\n");
			} else {
				write(connfd, buf, n);
				printf("Client: send filename %s\n", buf);

				memset(buf_data, 0, MAXLINE);
				if ((n = read(fd, buf_data, MAXLINE)) > 0) {
					write(connfd, buf_data, n);
					printf("Client: send %s file (%d bytes) to server.\n", buf, n);
				}

				close(fd);
			}
		}
	}

	close(connfd);
	return 0;
}


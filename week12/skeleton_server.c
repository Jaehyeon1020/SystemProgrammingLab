#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXLINE 80

int main (int argc, char *argv[]) {

    int n, listenfd, connfd, caddrlen;
    struct hostent *h;
    struct sockaddr_in saddr, caddr;
    char buf[MAXLINE];
    int port = atoi(argv[1]);

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket() failed.\n");
        exit(1);
    }

    memset((char *)&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(port);

    if (bind(listenfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
        printf("bind() failed.\n");
        exit(2);
    }

    if (listen(listenfd, 5) < 0) {
        printf("listen() failed.\n");
        exit(3);
    }

    fd_set readset, copyset;
    FD_ZERO(&readset);  /* initialize socket set */
    FD_SET(listenfd, &readset);
    int fdmax = listenfd, fdnum;

    while (1) {

        copyset = readset;

        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        if((fdnum = select(fdmax + 1, &copyset, NULL, NULL, &timeout)) < 0 ) {
            printf("select() failed.\n");
            exit(4);
        }

        
        if (fdnum == 0) {
            printf("Time out.\n");
            continue;
        }

        for (int i = 0; i < fdmax + 1; i++) {

            if (FD_ISSET(i, &copyset)) {

                if (i == listenfd) {
                    
                    caddrlen = sizeof(caddr);
                    if ((connfd = accept(listenfd, (struct sockaddr *)&caddr, (socklen_t *)&caddrlen)) < 0) {
                        printf ("accept() failed.\n");
                        continue;
                    }

                    FD_SET(connfd, &readset);

                    if (fdmax < connfd)
                        fdmax = connfd;

                }

                else {

                    if ((n = read(i, buf, MAXLINE)) > 0) {
                        printf ("got %d bytes from client.\n", n);
                        write(i, buf, n);
                    }

                    else {
                        FD_CLR(i, &readset);
                        printf("connection terminated.\n");
                        close(i);
                    }

                }

            }
           
        }

    }

    return 0;

}


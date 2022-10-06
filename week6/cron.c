#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdarg.h>

void main() {
    unsigned int pid;
    int fd;
    int fd0;
    int fd1;
    int fd2;

    if((pid = fork()) != 0)
        exit(0);
    if(setsid() < 0)
        exit(0);

    umask(0);
    close(0);
    close(1);
    close(2);
    fd0 = open("/dev/null", O_RDWR);
    fd1 = open("/dev/null", O_RDWR);
    fd2 = open("/dev/null", O_RDWR);

    fd = open("./simplecrontab", O_RDWR);

    while(1) {
        int childPid;
        int status;
        printf("In while\n");
        if((childPid = fork()) == 0) {
            char * simpleText;
            simpleText = fgets(fd);
            execl("/bin/sh", "-c", simpleText, NULL);
            exit(0);
        }
        waitpid(childPid, &status, WNOHANG);
        sleep(3);
    }

    return;
}
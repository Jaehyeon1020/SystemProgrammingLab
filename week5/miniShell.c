#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    while(1) {  // quit이 입력될때 까지 계속 입력 받음
		char *cmd = "";
		size_t size = 0;
        int i = 0;
        char *arg[100]; // execv() 인자 받을 문자열 배열
        char path[100];

        getline(&cmd, &size, stdin);
        cmd[strlen(cmd)-1] = '\0';

        if(strcmp(cmd, "quit") == 0) // quit 입력된 경우 종료
            break;

        char *token = strtok(cmd, " ");
        
        while(token != NULL) {
            arg[i++] = token;
            token = strtok(NULL, " ");
        }

        arg[i] = NULL; // arg[]에 인자 저장 완료
        sprintf(path, "/bin/%s", arg[0]); // execv() 첫 번째 인자 path에 저장
		
        
        if(fork() == 0) { // 자식프로세스 생성 -> 자식프로세스의 경우 fork() == 0
            execv(path,arg);
			exit(1);
        }
        wait(NULL);
    }

    return 0; // 확인
}

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

int send_sig_cnt; // 부모쪽에서 날린 시그널 수 (10고정)
int recv_sig_cnt; // 자식쪽에서 받은 시그널 수
int recv_ack_cnt; // 부모쪽에서 받은 시그널 수
pid_t pid;

void proc_exit_handler(int sig) /* SIGINT */
{
	printf("receiver: receiving signal : %d\n", recv_sig_cnt);
	return;
}

void recv_ack_handler(int sig) /* SIGUSR1 Parent */
{
	recv_ack_cnt++;
	return;
}

void send_sig_handler(int sig)	/* SIGALRM, 1초 후에 send_sig_handler 호출 */
{
	// recv_ack_cnt == send_sig_cnt ?
		// 같은 경우
			// 자식프로세스에게 sigint 날림
			// printf("all signals are sended\n");
			// exit(0);
		// 다른 경우
			//send_sig_cnt - recv_ack_cnt 개수 만큼 자식 프로세스에게 sigusr1날림 (kill 함수 이용)
	if(recv_ack_cnt == send_sig_cnt) {
		kill(pid, SIGINT); // sigint
		printf("all signals are sended\n");
		exit(0);
	}
	else {
		kill(pid, SIGUSR1); // sigusr1 자식에게 날림
	}


	printf("sender: sending %d signal\n", send_sig_cnt - recv_ack_cnt);

	/* ACK가 10개가 오지 않았을 수도 있으니까 (2개의 ACK가 왔다고 가정) */	
	alarm(1);
	return;
}

void send_ack_handler(int sig) /* SIGUSR1 날아왔을때 동작 정의 Child */
{
	// 부모프로세스에게 sigusr1 날림
	recv_sig_cnt++;
	printf("receiver: receive %d signal and sending acks\n", recv_sig_cnt);
	kill(pid, SIGUSR1);
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "The number of argument should be 2\n");
		exit(1);
	}

	send_sig_cnt = atoi(argv[1]);
	recv_ack_cnt = 0;
	recv_sig_cnt = 0;

	printf("sending signal: %d\n", send_sig_cnt);
	
	/* Install Signal Handler */

	if ((pid = fork()) == 0) { // 자식프로세스면
		pid = getppid(); // 부모 pid 저장
		signal(SIGINT, proc_exit_handler); // sigint받았을 때 등록
		signal(SIGUSR1, send_ack_handler); // sigusr1 받았을 때

		while (1);
	} else { // 부모프로세스면
		signal(SIGUSR1, recv_ack_handler); // sigusr1 왔을때 처리
		signal(SIGALRM, send_sig_handler);

		alarm(1);	/* 1초 후에 send_sig_handler 호출 */

		while (1);
	}

	return 0;
}

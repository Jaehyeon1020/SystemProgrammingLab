#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>

#define T_MSG 20

struct msg
{
	long msgtype;
	char text[512];
};

struct msg_rt
{
	long msgtype;
	char timestamp[256];
};

int main()
{
	/* 
	 * @ int uid: user ID
	 * @ int receiver_id: receiver's ID
	 */ 
	key_t key = ftok(".", 'a');
	int uid, qid, pid; 
	int receiver_id;

	printf("my id is \n");
	scanf("%d", &uid);
	getchar(); // flush

	printf("who to send ? \n");
	scanf("%d", &receiver_id); 
	getchar(); // flush  
		
	qid = msgget(key, IPC_CREAT | 0660);

	if ((pid = fork()) == 0) {
		while (1) {
			struct msg buf;
			struct msg_rt read_time;

			if(msgrcv(qid, (void *)&buf, sizeof(buf.text), uid, IPC_NOWAIT) > 0) {
				struct msg_rt t;
				time_t cur_time = time(NULL);

				printf("RECEIVED %s\n", buf.text);
				strcpy(t.timestamp, ctime(&cur_time));
				t.msgtype = T_MSG + receiver_id;
				msgsnd(qid, (void*)&t, sizeof(t.timestamp), 0);
			}

			if(msgrcv(qid, (void*)&read_time, sizeof(read_time.timestamp), uid+T_MSG, IPC_NOWAIT) > 0)
				printf("%d read message at: \t%s\n", receiver_id, read_time.timestamp);

		}
	} else {
		while (1) {
			char * line = NULL;
			size_t len = 0;
			struct msg buf;

			buf.msgtype = receiver_id;
			getline(&line, &len, stdin);
			if(!strcmp(line,"quit\n")) {
				kill(pid, SIGINT);
				msgctl(qid, IPC_RMID, NULL);
				free(line);
				exit(0);
			}
			strcpy(buf.text, line);

			if(msgsnd(qid, (void*)&buf, sizeof(buf.text), 0) < 0) {
				perror("msgsnd fail");
				exit(1);
			}
		}
	}

	return 0;
}

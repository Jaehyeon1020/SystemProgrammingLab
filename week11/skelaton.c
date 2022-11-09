#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

struct thread_data {
	int tid;
	int result;
	int ** matrix;
	int * vector;
};

int row;
int col;

void *thread_mvm(void *arg)
{
	int result = 0;
	struct thread_data * argument = (struct thread_data *)arg;
	int ** matrix = argument->matrix;
	int * vector = argument->vector;

	for (int i = 0; i < col; i++) {
		result += vector[i] * matrix[argument->tid][i];
	}
	argument->result = result;

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Usage: %s <row> <column>\n", argv[0]);
		exit(1);
	}

	row = atoi(argv[1]);
	col = atoi(argv[2]);

	pthread_t tid[row];
	struct thread_data t_data[row];

	srand(time(NULL));

	int ** matrix = NULL;
	matrix = (int **)malloc(sizeof(int*) * row);
	for (int i = 0; i < row; i++) {
		matrix[i] = (int *)malloc(sizeof(int) * col);
	}
	int * vector = (int *)malloc(sizeof(int) * col);

	/* 행렬 채우기 */
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			matrix[i][j] = rand() % 10;
		}
	}

	/* 벡터 채우기 */
	for (int i = 0; i < col; i++) {
		vector[i] = rand() % 10;
	}

	/* 행렬 출력 */
	printf("*** Matrix ***\n");
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			printf("[ %d ] ", matrix[i][j]);
		}
		printf("\n");
	}

	/* 벡터 출력 */
	printf("*** Vector ***\n");
	for (int i = 0; i < col; i++) {
		printf("[ %d ]\n", vector[i]);
	}
	printf("\n");

	/*  
	 *  Hint for Pthread Usage
	 *
	 *  pthread_create(&tid[i], NULL, thread_mvm, (void*)&t_data[i]);
	 *  
	 *  i means array index
	 * 
	 * 쓰레드 row개만큼 생성해야됨
	 */

	/* thread creation */
	for (int i = 0; i < row; i++) {
		t_data[i].tid = i;
		t_data[i].matrix = matrix;
		t_data[i].vector = vector;

		if (pthread_create(&tid[i], NULL, thread_mvm, (void*)&t_data[i])) {
			printf("ERROR: pthread creation failed.\n");
			exit(1);
		}
	}

	for (int i = 0; i < row; i++) {
		pthread_join(tid[i], NULL);
	}

	/* print result */
	printf("*** Result ***\n");
	for (int i = 0; i < row; i++) {
		printf("[ %d ]\n", t_data[i].result);
	}

	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int getSquaredDistance(int x, int y);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

int point_in_circle = 0;
int point_total = 0;
int r = 100;

void* thread(void *arg) {
    int x;
    int y;
    int squaredDistance;
    int pointNum = *((int*)arg); // number of points to generate

    srand(time(NULL));

    /* generate random points */
    for (int i = 0; i < pointNum; i++) {
		pthread_mutex_lock(&mutex2);
		point_total += 1;
		pthread_mutex_unlock(&mutex2);

        x = rand() % r;
        y = rand() % r;

        squaredDistance = getSquaredDistance(x, y);

        if (squaredDistance < r * r) {
            pthread_mutex_lock(&mutex);
            point_in_circle += 1;
            pthread_mutex_unlock(&mutex);
        }
    }

    pthread_exit(NULL);
}

/* get squared distance between (0,0) and (x,y) */
int getSquaredDistance(int x, int y) {
    return (x * x) + (y * y);
}

int main(int argc, char* argv[]) {
    int threadNum = atoi(argv[1]);
    int pointsPerThread = atoi(argv[2]);

    pthread_t tid[threadNum];
	int t;
    
    // thread creation
    for (t = 0; t < threadNum; t++) {
        if (pthread_create(&tid[t], NULL, thread, (void*)&pointsPerThread)) {
            printf("Thread creation failed\n");
            exit(1);
        }
    }
	for (t = 0; t < threadNum; t++) {
		pthread_join(tid[t], NULL);
	}

    const double PI = 4.0 * ((double)point_in_circle) / ((double)point_total);

    printf("PI : %lf\n", PI);
}




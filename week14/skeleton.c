#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define QSIZE 5
#define LOOP 30

typedef struct {
   int data[QSIZE];
   int put_index;
   int get_index;
   int count; // 현재 큐에 들어있는 데이터 개수
   pthread_mutex_t lock; // 큐 접근
   pthread_cond_t notfull; // producer가 꽉 차지 않은 상황을 기다림
   pthread_cond_t notempty; // consumer가 비어있지 않은 상황을 기다림
} queue_t;

void *produce (void *args);
void *consume (void *args);
void put_data (queue_t *q, int d);
void get_data (queue_t *q);

queue_t *qinit() 
{
   queue_t *q;

   q = (queue_t *) malloc(sizeof(queue_t));
   q->put_index = q->get_index = q->count = 0;
   pthread_mutex_init(&q->lock, NULL);
   pthread_cond_init(&q->notfull, NULL);
   pthread_cond_init(&q->notempty, NULL);

   return q;
}

void qdelete(queue_t *q) 
{
   pthread_mutex_destroy(&q->lock);
   pthread_cond_destroy(&q->notfull);
   pthread_cond_destroy(&q->notempty);
   free(q);
}

int main () 
{
   queue_t *q;
   pthread_t producer, consumer;

   q = qinit();

   pthread_create(&producer, NULL, produce, (void *)q);
   pthread_create(&consumer, NULL, consume, (void *)q);

   pthread_join (producer, NULL);
   pthread_join (consumer, NULL);
   
   qdelete(q);
}

void *produce(void *args) 
{
   int i, d;
   queue_t *q = (queue_t *)args;
   for (i = 0; i < LOOP; i++) {
      d = i;
      put_data(q, d);
   }
   pthread_exit(NULL);
}

void *consume(void *args) 
{
   int i;
   queue_t *q = (queue_t *)args;
   for (i = 0; i < LOOP; i++) {
      get_data(q);
   }
   pthread_exit(NULL);
}

/**
   1. put을 할때 putindex, count 바뀜 (mutex 필요)
   2. 만약 큐가 꽉 차면 -> cond_wait(mutex, notfull) 수행
   3. 데이터 enqueueing
   4. consumer에게 signal 날림(notempty)
   5. mutex_unlock
*/
void put_data(queue_t *q, int d) 
{
   pthread_mutex_lock(&(q->lock));

   /* 큐가 꽉 차있는 경우 */
   if (q->count == QSIZE) {
      pthread_cond_wait(&(q->notfull), &(q->lock));
   }

   /* 큐에 데이터 삽입 */
   q->data[q->put_index] = d;

   printf("put data %d to queue\n", d);

   /* put index, get index, count 조정 */
   q->put_index += 1;
   q->get_index = 0;
   q->count += 1;

   pthread_mutex_unlock(&(q->lock));
   pthread_cond_signal(&(q->notempty));
}

void get_data(queue_t *q)
{
   pthread_mutex_lock(&(q->lock));

   int data; // 큐에서 가져온 데이터 

   /* 큐가 비어있는 경우 */
   if (q->count == 0) {
      pthread_cond_wait(&(q->notempty), &(q->lock));
   }

   /* 큐에서 데이터 가져옴 */
   data = q->data[q->get_index];

   printf("got data %d from queue\n", data);

   /* put index, get index, count 조정 */
   q->get_index += 1;
   q->put_index -= 1;
   q->count -= 1;

   pthread_mutex_unlock(&(q->lock));
   pthread_cond_signal(&(q->notfull));
}


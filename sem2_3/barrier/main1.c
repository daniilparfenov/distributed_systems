#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
int thread_count;

/*
pthread_cond_wait(&cond, &mutex):
      pthread_mutex_unlock(&mutex);
      wait_on_signal(&cond);
      pthread_mutex_lock(&mutex);

pthread_cond_signal(&cond);
pthread_cond_broadcast(&cond);

lock mutex
if condition has occured
      signal thread(s)
else
      unlock mutex
      wait_on_signal
      lock_mutex
unlock mutex
*/

#define NBARRIERS 1
int counter;
pthread_mutex_t mutex;

sem_t* sem_barrier;
sem_t sem_count;

void* routine(void* rank) {
    long my_rank = (long)rank;
    for (int barrier = 0; barrier < NBARRIERS; ++barrier) {
        sem_wait(&sem_count);
        if (counter == thread_count - 1) {
            counter = 0;
            sem_post(&sem_count);
            for (int i = 0; i < thread_count - 1; ++i)
                sem_post(&sem_barrier[barrier]);
        } else {
            counter++;
            sem_post(&sem_count);
            sem_wait(&sem_barrier[barrier]);
        }
    }
    return NULL;
}

int main(int argc, char** argv) {

    thread_count = strtol(argv[1], NULL, 10);
    pthread_mutex_init(&mutex, NULL);

    sem_barrier = (sem_t*)malloc(NBARRIERS * sizeof(sem_t));

    for (int i = 0; i < NBARRIERS; ++i)
        sem_init(&sem_barrier[i], 0, 0);
    sem_init(&sem_count, 0, 1);

    pthread_t* thread_handles = malloc(thread_count * sizeof(pthread_t));

    double start, end;
    GET_TIME(start);
    for (long i = 0; i < thread_count; ++i)
        pthread_create(&thread_handles[i], NULL, routine, (void*)i);

    for (int i = 0; i < thread_count; ++i)
        pthread_join(thread_handles[i], NULL);
    GET_TIME(end);
    printf("%lf\n", end - start);
    free(thread_handles);

    for (int i = 0; i < NBARRIERS; ++i)
        sem_destroy(&sem_barrier[i]);
    sem_destroy(&sem_count);
    free(sem_barrier);
    pthread_mutex_destroy(&mutex);
}

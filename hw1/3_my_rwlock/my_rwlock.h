#ifndef MY_RWLOCK_H
#define MY_RWLOCK_H

#include <pthread.h>
#include <stdbool.h>

typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t readersCond;
    pthread_cond_t writersCond;

    size_t activeReadersCnt;
    size_t waitingReadersCnt;
    size_t waitingWritersCnt;

    bool hasActiveWriter;
} my_rwlock_t;

void my_rwlock_init(my_rwlock_t* lock);
void my_rwlock_destroy(my_rwlock_t* lock);
void my_rwlock_rdlock(my_rwlock_t* lock);
void my_rwlock_wrlock(my_rwlock_t* lock);
void my_rwlock_unlock(my_rwlock_t* lock);

#ifdef MY_RWLOCK_IMPLEMENTATION
void my_rwlock_init(my_rwlock_t* lock)
{
    pthread_mutex_init(&lock->mutex, NULL);
    pthread_cond_init(&lock->readersCond, NULL);
    pthread_cond_init(&lock->writersCond, NULL);
    lock->activeReadersCnt = 0;
    lock->waitingReadersCnt = 0;
    lock->waitingWritersCnt = 0;
    lock->hasActiveWriter = false;
}

void my_rwlock_destroy(my_rwlock_t* lock)
{
    pthread_mutex_destroy(&lock->mutex);
    pthread_cond_destroy(&lock->readersCond);
    pthread_cond_destroy(&lock->writersCond);
}

void my_rwlock_rdlock(my_rwlock_t* lock)
{
    pthread_mutex_lock(&lock->mutex);
    lock->waitingReadersCnt++;
    while (lock->hasActiveWriter || lock->waitingWritersCnt > 0)
    {
        pthread_cond_wait(&lock->readersCond, &lock->mutex);
    }
    lock->waitingReadersCnt--;
    lock->activeReadersCnt++;
    pthread_mutex_unlock(&lock->mutex);
}

void my_rwlock_wrlock(my_rwlock_t* lock)
{
    pthread_mutex_lock(&lock->mutex);

    lock->waitingWritersCnt++;
    while (lock->hasActiveWriter || lock->activeReadersCnt > 0)
    {
        pthread_cond_wait(&lock->writersCond, &lock->mutex);
    }
    lock->waitingWritersCnt--;
    lock->hasActiveWriter = true;
    pthread_mutex_unlock(&lock->mutex);
}

void my_rwlock_unlock(my_rwlock_t* lock)
{
    pthread_mutex_lock(&lock->mutex);
    if (lock->hasActiveWriter)
    {
        lock->hasActiveWriter = false;

        if (lock->waitingWritersCnt > 0)
        {
            pthread_cond_signal(&lock->writersCond);
        }
        else
        {
            pthread_cond_broadcast(&lock->readersCond);
        }
    }
    else
    {
        lock->activeReadersCnt--;
        if (lock->activeReadersCnt == 0 && lock->waitingWritersCnt > 0)
        {
            pthread_cond_signal(&lock->writersCond);
        }
    }
    pthread_mutex_unlock(&lock->mutex);
}
#endif  // #ifdef MY_RWLOCK_IMPLEMENTATION

#endif  // #ifndef MY_RWLOCK_H

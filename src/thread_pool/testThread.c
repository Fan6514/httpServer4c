#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "thread_pool.h"

void taskFunc(void* arg)
{
    int num = *(int*)arg;
    printf("thread %ld is working, number = %d\n",
        pthread_self(), num);
    sleep(10);
}

int main()
{
    ThreadPool *pThreadPool = NULL;

    pThreadPool = threadPoolCreate(1, 1, 1);
    for (int i = 0; i < 100; ++i)
    {
        int* num = (int*)malloc(sizeof(int));
        *num = i + 100;
        threadPoolAddTask(pThreadPool, taskFunc, num);
    }

    sleep(30);
    return 0;
}
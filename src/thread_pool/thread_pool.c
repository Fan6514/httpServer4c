#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "../include/util.h"
#include "thread_pool.h"

ThreadPool * threadPoolCreate(int taskCapacity, int poolSize, int poolMinSize)
{
    int ret = 0;
    ThreadPool *pThreadPool = NULL;

    /* 初始化线程池 */
    GET_MEMORY(pThreadPool, ThreadPool, sizeof(ThreadPool), error);
    GET_MEMORY(pThreadPool->workIDs, pthread_t, sizeof(pthread_t)*poolSize, error);
    pThreadPool->poolSize = poolSize;
    pThreadPool->minNum = poolMinSize;
    pThreadPool->liveNum = poolMinSize;
    pThreadPool->busyNum = 0;
    pThreadPool->exitNum = 0;
    pThreadPool->poolState = RUNNING;

    /* 初始化任务队列 */
    GET_MEMORY(pThreadPool->taskList, Task, sizeof(Task)*taskCapacity, error);
    pThreadPool->taskCapacity = taskCapacity;
    pThreadPool->taskSize = 0;
    pThreadPool->taskFront = 0;
    pThreadPool->taskTail = 0;

    /* 初始化锁 */
    ret = pthread_mutex_init(&pThreadPool->mutexPool, NULL);
    CHECK_RETURN(ret, SUCCESS, "pthread_mutex_init error.\n");
    /* 初始化条件变量 */
    ret = pthread_cond_init(&pThreadPool->condPool, NULL);
    CHECK_RETURN(ret, SUCCESS, "pthread_cond_init error.\n");
    ret = pthread_cond_init(&pThreadPool->notEmpty, NULL);
    CHECK_RETURN(ret, SUCCESS, "pthread_cond_init error.\n");
    ret = pthread_cond_init(&pThreadPool->notFull, NULL);
    CHECK_RETURN(ret, SUCCESS, "pthread_cond_init error.\n");

    /* 创建线程 */
    ret = pthread_create(&pThreadPool->managerID, NULL, manager, pThreadPool);
    CHECK_RETURN(ret, SUCCESS, "pthread_create error.\n");
    for (int i = 0; i < poolMinSize; i++)
    {
        ret = pthread_create(&pThreadPool->workIDs[i], NULL, work, pThreadPool);
        CHECK_RETURN(ret, SUCCESS, "pthread_create error.\n");
    }

    return pThreadPool;

error:
    REL_MEMORY(pThreadPool);
    REL_MEMORY(pThreadPool->workIDs);
    REL_MEMORY(pThreadPool->taskList);
    return NULL;
}

int threadPoolDestroy(ThreadPool *pThreadPool)
{
    int ret = 0;

    CHECK_POINT(pThreadPool);

    /* 关闭线程池 */
    pThreadPool->poolState = STOP;

    /* 阻塞管理线程 */
    pthread_join(pThreadPool->managerID, NULL);
    /* 唤醒阻塞线程，让线程自杀 */
    for (int i = 0; i < pThreadPool->liveNum; ++i)
    {
        pthread_cond_signal(&pThreadPool->notEmpty);
    }

    /* 销毁锁 */
    pthread_mutex_destroy(&pThreadPool->mutexPool);
    pthread_cond_destroy(&pThreadPool->condPool);
    pthread_cond_destroy(&pThreadPool->notEmpty);
    pthread_cond_destroy(&pThreadPool->notFull);

    /* 释放内存 */
    REL_MEMORY(pThreadPool->workIDs);
    REL_MEMORY(pThreadPool->taskList);
    REL_MEMORY(pThreadPool);

    return SUCCESS;
}

int threadPoolAddTask(ThreadPool *pThreadPool, void(*func)(void*), void *arg)
{
    int ret = 0;

    CHECK_POINT(pThreadPool);
    CHECK_POINT(func);
    CHECK_POINT(arg);

    pthread_mutex_lock(&pThreadPool->mutexPool);
    /* 如果任务队列已满，则阻塞生产者线程 */
    while (pThreadPool->taskSize == pThreadPool->taskCapacity 
            && RUNNING == pThreadPool->poolState)
    {
        pthread_cond_wait(&pThreadPool->notFull, &pThreadPool->mutexPool);
    }

    /* 如果线程池关闭则退出 */
    if(RUNNING != pThreadPool->poolState)
    {
        pthread_mutex_unlock(&pThreadPool->mutexPool);
        return SUCCESS;
    }

    /* 添加任务到任务队列 */
    pThreadPool->taskList[pThreadPool->taskTail].function = func;
    pThreadPool->taskList[pThreadPool->taskTail].arg = arg;
    pThreadPool->taskTail = (pThreadPool->taskTail + 1) % pThreadPool->taskCapacity;
    pThreadPool->taskSize++;

    pthread_cond_signal(&pThreadPool->notEmpty);
    pthread_mutex_unlock(&pThreadPool->mutexPool);
    return SUCCESS;
}

int threadPoolBusyNum(ThreadPool *pThreadPool)
{
    int busyNum = 0;
    pthread_mutex_lock(&pThreadPool->mutexPool);
    busyNum = pThreadPool->busyNum;
    pthread_mutex_unlock(&pThreadPool->mutexPool);

    return busyNum;
}

int threadPoolLiveNum(ThreadPool *pThreadPool)
{
    int liveNum = 0;
    pthread_mutex_lock(&pThreadPool->mutexPool);
    liveNum = pThreadPool->liveNum;
    pthread_mutex_unlock(&pThreadPool->mutexPool);
    
    return liveNum;
}

void *work(void* arg)
{
    int ret = 0;
    Task task;
    ThreadPool *pThreadPool = NULL;

    pThreadPool = (ThreadPool*)arg;
    while(1)
    {
        pthread_mutex_lock(&pThreadPool->mutexPool);

        /* 任务队列为空，阻塞消费者线程 */
        while (pThreadPool->taskSize == 0 && RUNNING == pThreadPool->poolState)
        {
            pthread_cond_wait(&pThreadPool->notEmpty, &pThreadPool->mutexPool);

            /* 需要退出线程 */
            if(pThreadPool->exitNum > 0)
            {
                pThreadPool->exitNum--;
                if (pThreadPool->liveNum > pThreadPool->minNum)
                {
                    pThreadPool->liveNum--;
                    pthread_mutex_unlock(&pThreadPool->mutexPool);
                    threadExit(pThreadPool);
                }
            }
        }

        /* 需要关闭线程池 */
        if (STOP == pThreadPool->poolState)
        {
            pthread_mutex_unlock(&pThreadPool->mutexPool);
            threadExit(pThreadPool);
        }

        /* 从任务队列队头取任务 */
        task.function = pThreadPool->taskList[pThreadPool->taskFront].function;
        task.arg = pThreadPool->taskList[pThreadPool->taskFront].arg;
        pThreadPool->taskFront = (pThreadPool->taskFront + 1) % pThreadPool->taskCapacity;
        pThreadPool->taskSize--;

        pthread_cond_signal(&pThreadPool->notFull);
        pthread_mutex_unlock(&pThreadPool->mutexPool);
        /* 处理忙线程数量，开始任务 */
        pthread_mutex_lock(&pThreadPool->mutexBusy);
        pThreadPool->busyNum++;
        pthread_mutex_unlock(&pThreadPool->mutexBusy);

        task.function(task.arg);

        /* 任务结束 */
        pthread_mutex_lock(&pThreadPool->mutexBusy);
        pThreadPool->busyNum--;
        pthread_mutex_unlock(&pThreadPool->mutexBusy);
    }
    return NULL;
}

void *manager(void *arg)
{
    THREAD_STATE state;
    ThreadPool *pThreadPool = NULL;

    pThreadPool = (ThreadPool*)arg;
    while (RUNNING == pThreadPool->poolState)
    {
        /* 每3s检测一次 */
        sleep(3);

        /* 任务调度 */
        state = getCurrentState(pThreadPool);
        switch(state)
        {
            case REALLOCATION:
                addThread(pThreadPool);
                break;
            case DELETE:
                delThread(pThreadPool);
                break;
            default:
                break;
        }
    }
    return NULL;
}

void addThread(ThreadPool *pThreadPool)
{
    int count = 0;
    int ret = 0;
    pthread_mutex_lock(&pThreadPool->mutexPool);

    for (int i = 0; i < pThreadPool->poolSize && count < PROCESS_THREAD_NUM
        && pThreadPool->liveNum < pThreadPool->poolSize; ++i)
    {
        if (pThreadPool->workIDs[i] == 0)
        {
            ret = pthread_create(&pThreadPool->workIDs[i], NULL, work, pThreadPool);
            CHECK_RETURN(ret, SUCCESS, "pthread_create error.\n");
            count++;
            pThreadPool->liveNum++;
        }
    }

    pthread_mutex_unlock(&pThreadPool->mutexPool);
}

void delThread(ThreadPool *pThreadPool)
{
    pthread_mutex_lock(&pThreadPool->mutexPool);
    pThreadPool->exitNum = PROCESS_THREAD_NUM;
    pthread_mutex_unlock(&pThreadPool->mutexPool);

    /* 唤醒线程，让线程自己结束 */
    for (int i = 0; i < PROCESS_THREAD_NUM; ++i)
    {
        pthread_cond_signal(&pThreadPool->notEmpty);
    }
}

THREAD_STATE getCurrentState(ThreadPool *pThreadPool)
{
    int taskSize = 0;
    int liveNum = 0;
    int busyNum = 0;
    THREAD_STATE state;

    pthread_mutex_lock(&pThreadPool->mutexPool);
    taskSize = pThreadPool->taskSize;
    liveNum = pThreadPool->liveNum;
    pthread_mutex_unlock(&pThreadPool->mutexPool);

    pthread_mutex_lock(&pThreadPool->mutexBusy);
    busyNum = pThreadPool->busyNum;
    pthread_mutex_unlock(&pThreadPool->mutexBusy);

    /* 添加线程 - 任务的个数 > 存活的线程个数 && 存活的线程数 < 最大线程数 */
    if (taskSize > liveNum && liveNum < pThreadPool->poolSize)
    {
        state = REALLOCATION;
    }
    /* 销毁线程 - 忙的线程*2 < 存活的线程数 && 存活的线程 > 最小线程数 */
    else if (busyNum*2 < liveNum && liveNum > pThreadPool->minNum)
    {
        state = DELETE;
    }

    return state;
}

void threadExit(ThreadPool *pThreadPool)
{
    int ret = 0;
    pthread_t tid = 0;

    CHECK_POINT_NORTN(pThreadPool);

    tid = pthread_self();
    for (int i = 0; i < pThreadPool->poolSize; ++i)
    {
        if (pThreadPool->workIDs[i] == tid)
        {
            pThreadPool->workIDs[i] = 0;
            break;
        }
    }

    pthread_exit(NULL);
}
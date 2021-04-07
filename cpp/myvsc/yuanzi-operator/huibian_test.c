#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define THREAD_COUNTER 2
#define COUNTER  10000000
int s_i;

static int lxx_atomic_add(int *ptr, int increment)
{
    int old_value = *ptr;
    __asm__ volatile("lock; xadd %0, %1 \n\t"
                     : "=r"(old_value), "=m"(*ptr)
                     : "0"(increment), "m"(*ptr)
                     : "cc", "memory");
    return *ptr;
}

void* increfunc(void *arg) {
    int i;
    for (i = 0; i < COUNTER; i++) {
        lxx_atomic_add(&s_i, 1);
    }
    printf("thread:%d finish\n", *(int*)arg);
    pthread_exit(NULL);
    return NULL;
}



int main() {
    int i;
    int id[THREAD_COUNTER];
    pthread_t tid[THREAD_COUNTER];
    for (i = 0; i < THREAD_COUNTER; i++) {
        id[i] = i;
        pthread_create(&tid[i], NULL, increfunc, &id[i]);
    }

    for (i = 0; i < THREAD_COUNTER; i++) {
        pthread_join(tid[i], NULL);
    }
    printf("expect result:%d, actual value:%d\n", THREAD_COUNTER * COUNTER, s_i);
    return 0;
}

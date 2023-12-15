#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int eventReady = 0;

void* provider(void* arg) {
    while (1) {
        sleep(1);
        pthread_mutex_lock(&mutex);

        if(eventReady == 1){
            pthread_mutex_unlock(&mutex);
            printf("provider: событие не обработанно\n");
            continue;
        }

        eventReady = 1;
        printf("provider: отправлено событие\n");

        pthread_cond_signal(&condition);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* consumer(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        while (eventReady == 0) {
            pthread_cond_wait(&condition, &mutex);
        }

        eventReady = 0;
        printf("consumer: получено событие\n\n");

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t producerThread, consumerThread;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condition, NULL);

    pthread_create(&producerThread, NULL, provider, NULL);
    pthread_create(&consumerThread, NULL, consumer, NULL);

    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condition);

    return 0;
}

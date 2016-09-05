#include <semaphore.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

void* func(void* sem) {
	sem_wait((sem_t*) sem);
	printf("HOHO! from thread something! \n");
	sleep(3);
	sem_post((sem_t*) sem);
	return NULL;
}


int main() {
	
	sem_t sem;
	sem_init(&sem, 1, 3);
	
	
	pthread_t one;
    pthread_t two;
    pthread_t three;
    pthread_t four;
    pthread_t five;

    pthread_create(&one, NULL, func, (void*) &sem);
    pthread_create(&two, NULL, func, (void*) &sem);
    pthread_create(&three, NULL, func, (void*) &sem);
    pthread_create(&four, NULL, func, (void*) &sem);
    pthread_create(&five, NULL, func, (void*) &sem);

    pthread_join(one, NULL);
    pthread_join(two, NULL);
    pthread_join(three, NULL);
    pthread_join(four, NULL);
	pthread_join(five, NULL);
    
	return 0;
}

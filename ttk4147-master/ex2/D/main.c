#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int running = 1;
int var1 = 0;
int var2 = 0;

pthread_mutex_t mutex;

void* thread1() {
	while(running) {
		pthread_mutex_lock(&mutex);
		var1++;
		var2 = var1;
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

void* thread2() {
	for (int i = 0; i < 20; i++) {
		pthread_mutex_lock(&mutex);
		printf("Number 1 is %d, number 2 is %d\n", var1, var2);
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
	running = 0;
	return NULL;
}

int main() {
	pthread_t one;
	pthread_t two;
	
	pthread_mutex_init(&mutex, NULL);
	
	pthread_create(&one, NULL, thread1, NULL);
	pthread_create(&two, NULL, thread2, NULL);
	
	pthread_join(one, NULL);
	pthread_join(two, NULL);
	
	return 0;
}



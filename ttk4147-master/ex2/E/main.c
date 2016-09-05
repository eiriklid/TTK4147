#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define NUMPHILS 5
#define PICK 0
#define PUT 1

pthread_mutex_t forks[NUMPHILS];

void left(long phil, int action) {
	if (phil == 0) {
		if (action == PICK)
			pthread_mutex_lock(&forks[4]);
		else if (action == PUT)
			pthread_mutex_unlock(&forks[4]);
	} else {
		if (action == PICK)
			pthread_mutex_lock(&forks[phil-1]);
		else if (action == PUT)
			pthread_mutex_unlock(&forks[phil-1]);
	}
}

void right(long phil, int action) {
	if (action == PICK)
		pthread_mutex_lock(&forks[phil]);
	else if (action == PUT)
		pthread_mutex_unlock(&forks[phil]);
}

void eat(long phil) {
	printf("Philosopher %ld is eating\n", phil);
	usleep(10000);
}

void* philFunc(void* phil) {
	while(1) {
		if ((long) phil == 1) {
			right((long) phil, PICK);
			left((long) phil, PICK);
			eat((long) phil);
			left((long) phil, PUT);
			right((long) phil, PUT);		
		}
		else {
			left((long) phil, PICK);
			right((long) phil, PICK);
			eat((long) phil);
			right((long) phil, PUT);
			left((long) phil, PUT);
		}
	}
	return NULL;
}

int main() {
	pthread_t phils[NUMPHILS];
	
	for (long i = 0; i < NUMPHILS; i++) {
		pthread_create(&phils[i], NULL, philFunc, (void*) i);
	}
	
	for (int i = 0; i < NUMPHILS; i++) {
		pthread_join(phils[i], NULL);
	}
	return 0;
}

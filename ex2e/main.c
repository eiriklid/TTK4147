#include<unistd.h>
#include<stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include<semaphore.h>
#include <sys/times.h>

#define NUM_PHIL 5


typedef struct{
	int position;
	pthread_mutex_t *forks;
}pos_t;

pthread_mutex_t waiter;



void think(int position)
{
	printf("Philosopher %d thinking...\n",position);
}
void eat(int position)
{
	printf("Philosopher %d eating...\n",position);
}

void *philosopher(void *pos){
	
	pos_t self = *(pos_t*)pos;
	printf("Philosopher %i running\n", self.position );
	while(1){
		pthread_mutex_lock(&waiter);
		pthread_mutex_lock(&self.forks[self.position]);
		pthread_mutex_lock(&self.forks[(self.position + 1) % NUM_PHIL]);
		eat(self.position);
		pthread_mutex_unlock(&self.forks[self.position]);
		pthread_mutex_unlock(&self.forks[(self.position + 1) % NUM_PHIL]);
		pthread_mutex_unlock(&waiter);
		
		//think(self.position);
		
		usleep(10);
	}

}


int main(int argc, char **argv){

	if(pthread_mutex_init(&waiter, NULL) != 0){
		perror("mutex init");
		exit(0);
	}
	
	pthread_mutex_t forks[NUM_PHIL];
	pthread_t phil[NUM_PHIL];

	int i;
	for (i = 0; i < NUM_PHIL; i++)
	{
		if(pthread_mutex_init(&forks[i], NULL) != 0){
			perror("mutex init");
			exit(0);
		}
	}
	

	for(i = 0; i < NUM_PHIL; i++){
		pos_t* arg= malloc(sizeof(pos_t));
		arg-> position = i;
		arg->forks = forks;

		printf("Creating thread %i \n", i );
		pthread_create(&phil[i], NULL, philosopher, (void *)arg);
	}

	for (i = 0; i < NUM_PHIL; i++){
		pthread_join(phil[i], NULL);
		
	}
	return 0;

}


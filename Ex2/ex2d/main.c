
#include<unistd.h>
#include<stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/times.h>


int running = 1;
int var1 = 0;
int var2 = 0;
pthread_mutex_t lock;

void *thread1(){
	while (running){
		pthread_mutex_lock(&lock);
		var1++;
		var2 = var1;
		pthread_mutex_unlock(&lock);
	}
}

void *thread2(){
	int i;
	for(i = 0; i < 20; i++){
		pthread_mutex_lock(&lock);
		printf("Number 1 is %i  number 2 is %i\n",var1, var2);
		pthread_mutex_unlock(&lock);
		usleep(100000);
	}
	running = 0;
}


int main(int argc, char **argv){

	if (pthread_mutex_init(&lock, NULL) != 0){
		perror("mutex init");
		exit(0);
	}

	pthread_t pth1;
	pthread_t pth2;

	pthread_create(&pth1, NULL, thread1, NULL);
	pthread_create(&pth2, NULL, thread2, NULL);

	pthread_join(pth1, NULL);
	pthread_join(pth2, NULL);	

	return 0;
}
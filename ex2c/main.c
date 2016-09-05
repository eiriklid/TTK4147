#include<unistd.h>
#include<stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include<semaphore.h>
#include <sys/times.h>

sem_t mySem;


void *thread(){
	int i;
	for (i = 0; i < 3; ++i)
	{
		sem_wait(&mySem);
		printf("Thread number %i\n", i);
		usleep(100000);
		sem_post(&mySem);

	}
}

int main(int argc, char **argv){
	if ( sem_init(&mySem,0,3) < 0)
	{
		perror( "semaphore init");
		exit(0);
	}

	pthread_t pth1;
	pthread_t pth2;
	pthread_t pth3;
	pthread_t pth4;
	pthread_t pth5;


	pthread_create(&pth1, NULL, thread, NULL);
	pthread_create(&pth2, NULL, thread, NULL);
	pthread_create(&pth3, NULL, thread, NULL);
	pthread_create(&pth4, NULL, thread, NULL);
	pthread_create(&pth5, NULL, thread, NULL);


	pthread_join(pth1, NULL);
	pthread_join(pth2, NULL);	
	pthread_join(pth3, NULL);	
	pthread_join(pth4, NULL);	
	pthread_join(pth5, NULL);	
	return 0;

}



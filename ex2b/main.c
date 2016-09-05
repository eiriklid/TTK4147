#include<unistd.h>
#include<stdio.h>
#include <pthread.h>


int global = 0;


void *increment( void* data){
	data +=1;
	global +=1;
	printf("process global: %i\n", global );
	printf("process local: %i\n", data);
	
}

int main(int argc, char **argv){
	int* local = 0;
	
	pthread_t pth1;
	pthread_t pth2;

	pthread_create(&pth1, NULL, increment, (void*) local);
	pthread_create(&pth2, NULL, increment, (void*) local);

	pthread_join(pth1, NULL);
	pthread_join(pth2, NULL);	
	return 0;

}

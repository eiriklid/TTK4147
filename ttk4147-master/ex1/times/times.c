#include <sys/times.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void busy_wait_delay(int seconds) 
{ 
    int i, dummy; 
    int tps = sysconf(_SC_CLK_TCK); 
	clock_t start; 
    struct tms exec_time; 
 	times(&exec_time); 
	start = exec_time.tms_utime; 
    while( (exec_time.tms_utime - start) < (seconds * tps)) 
	{ 
		for(i=0; i<1000; i++) 
  		{ 
        	dummy = i; 
  		} 
  		times(&exec_time); 
 	}  
} 

void* print() {
    printf("start\n");
    sleep(5);
    printf("end\n");

    return NULL;
}


void* print2() {
	printf("start\n");
    busy_wait_delay(5);
    printf("end\n");

	return NULL;
}

int main() {
    pthread_t one;
    pthread_t two;

    pthread_create(&one, NULL, print2, NULL);
    pthread_create(&two, NULL, print2, NULL);

    pthread_join(one, NULL);
    pthread_join(two, NULL);

    return 0;
}



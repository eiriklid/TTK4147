#include "io.h"
#include <unistd.h>
#include <pthread.h>
#include <sys/times.h>
#include <sched.h>
#include <time.h>

int set_cpu(int cpu_number)
{
	// setting cpu set to the selected cpu
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);
	// set cpu set to current thread and return
	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void timespec_add_us(struct timespec *t, long us)
{
	// add microseconds to timespecs nanosecond counter
	t->tv_nsec += us*1000;
	// if wrapping nanosecond counter, increment second counter
	if (t->tv_nsec > 1000000000)
	{
		t->tv_nsec = t->tv_nsec - 1000000000;
		t->tv_sec += 1;
	}
}




void *testA(){
	set_cpu(1);
	io_write(1,1);
	struct timespec next;
	clock_gettime(CLOCK_REALTIME, &next);

	while(1){
		if (io_read(1) == 0){
			io_write(1,0);
			usleep(5);
			io_write(1,1);	
		}

	timespec_add_us(&next,100);
	clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &next, NULL);
		
	}
}

void *testB(){
	set_cpu(1);
	io_write(2,1);
	struct timespec next;
	clock_gettime(CLOCK_REALTIME, &next);

	while(1){
		if (io_read(2) == 0){
			io_write(2,0);
			usleep(5);
			io_write(2,1);	
		}

	timespec_add_us(&next,100);
	clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &next, NULL);
		
	}
}


void *testC(){
	set_cpu(1);
	io_write(3,1);
	struct timespec next;
	clock_gettime(CLOCK_REALTIME, &next);
	while(1){
		if (io_read(3) == 0){
			io_write(3,0);
			usleep(5);
			io_write(3,1);	
		}
	timespec_add_us(&next,100);
	clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &next, NULL);
		
	}
}


void *busyWait(){
	int sum = 0;
	while(1){
		sum+= 1;
	}
}




int main (int argc, char **argv){

	io_init();



	struct timespec next2;
	struct timespec next3;


	pthread_t pth1;
	pthread_t pth2;
	pthread_t pth3;
	pthread_t pth4;
	pthread_t pth5;
	pthread_t pth6;
	pthread_t pth7;
	pthread_t pth8;
	pthread_t pth9;
	pthread_t pth10;
	pthread_t pth11;
	pthread_t pth12;
	pthread_t pth13;

	pthread_create(&pth1, NULL, testA, NULL);
	pthread_create(&pth2, NULL, testB, NULL);
	pthread_create(&pth3, NULL, testC, NULL);
	pthread_create(&pth4, NULL, busyWait, NULL);
	pthread_create(&pth5, NULL, busyWait, NULL);
	pthread_create(&pth6, NULL, busyWait, NULL);
	pthread_create(&pth7, NULL, busyWait, NULL);
	pthread_create(&pth8, NULL, busyWait, NULL);
	pthread_create(&pth9, NULL, busyWait, NULL);
	pthread_create(&pth10, NULL, busyWait, NULL);
	pthread_create(&pth11, NULL, busyWait, NULL);
	pthread_create(&pth12, NULL, busyWait, NULL);
	pthread_create(&pth13, NULL, busyWait, NULL);

	pthread_join(pth1, NULL);
	pthread_join(pth2, NULL);
	pthread_join(pth3, NULL);
	pthread_join(pth4, NULL);
	pthread_join(pth5, NULL);
	pthread_join(pth6, NULL);
	pthread_join(pth7, NULL);
	pthread_join(pth8, NULL);
	pthread_join(pth9, NULL);
	pthread_join(pth10, NULL);
	pthread_join(pth11, NULL);
	pthread_join(pth12, NULL);
	pthread_join(pth13, NULL);
	
	return 0;
}
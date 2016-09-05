#include "io.h"
#include <unistd.h>
#include <pthread.h>
#include <sched.h>

#define CPU_CORE 1

int set_cpu(int cpu_number)
{
	// setting cpu set to the selected cpu
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);
	// set cpu set to current thread and return
	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void* test(void* num_param) {
	int num = (long) num_param;
	set_cpu(CPU_CORE);
	io_write(num, 1);
	while(1) {
		while (io_read(num));

		io_write(num, 0);
		usleep(5);
		io_write(num, 1);
	}
}

void* disturbance() {
	set_cpu(CPU_CORE);
	double i, j, k;
	while(1) {
		i++;
		j++;
		k = i*j;
		k++;
	}
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

void* periodicTest(void* num_param) {
	int num = (long) num_param;
	set_cpu(CPU_CORE);
	io_write(num, 1);
	struct timespec next;
	clock_gettime(CLOCK_REALTIME, &next);
	while(1) {
		while (io_read(num));

		io_write(num, 0);
		
		
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &next, NULL);
		timespec_add_us(&next, 100);
		io_write(num, 1);
	}
}

void* periodicPrint() {
	while(1) {
		printf("hei\n");
		struct timespec next;
		clock_gettime(CLOCK_REALTIME, &next);
	
		timespec_add_us(&next, 500000);
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &next, NULL);
	}
}

int main() {
    io_init();
    pthread_t tests[3];
    pthread_t disturbances[10];
    long i;
    for (i = 1; i <= 3; i++) {
    	pthread_create(&tests[i-1], NULL, periodicTest, (void*) i);
    }
    
    for (i = 0; i < 10; i++) {
    	pthread_create(&disturbances[i], NULL, disturbance, NULL);
    }
    
    for (i = 0; i < 3; i++) {
		pthread_join(tests[i], NULL);
	}
	
	for (i = 0; i < 10; i++) {
		pthread_join(disturbances[i], NULL);
	}
	
	return 0;
}

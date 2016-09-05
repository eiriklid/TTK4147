#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sched.h>
#include <pthread.h>

#include <native/task.h>
#include <native/timer.h>

#include  <rtdk.h>

#include <sys/io.h>
#include "io.h"

void funksjon()
{
	printf("hoho\n");
}


void periodicPrint()
{
	rt_task_set_periodic(NULL, TM_NOW, 1000000000);
	
	while(1)
	{
		printf("Maja-hi, Maja-ho, Maja-ha, Maja-ha-ha\n");
		
		rt_task_wait_period(NULL);
	}
}

int set_cpu(int cpu_number)
{
	// setting cpu set to the selected cpu
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);
	// set cpu set to current thread and return
	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void* disturbance() {
	set_cpu(1);
	double i, j, k;
	while(1) {
		i++;
		j++;
		k = i*j;
		k++;
	}
}


void catch_signal(int sig) {
}


void wait_for_ctrl_c() {
	signal(SIGTERM, catch_signal);
	signal(SIGINT, catch_signal);
  // wait for SIGINT (CTRL-C) or SIGTERM signal
	pause();
	printf("     HADE :D\n");
}




void periodicTest(void* num_param) {
	int num = (long) num_param;
	io_write(num, 1);
	rt_task_set_periodic(NULL, TM_NOW, 1);
	
	while(1) {
		while (io_read(num)) {
			rt_task_wait_period(NULL);
		}
		io_write(num, 0);
		usleep(5);
		io_write(num, 1);
	}
}




int main ()
{
	io_init();
	RT_TASK test[3];
	mlockall(MCL_CURRENT|MCL_FUTURE);
	char name[10];
	long i;
	for (i = 1; i <= 3; i++) {
		sprintf(name, "test%lu", i);
    	rt_task_create(&test[i-1], name, 0, i, T_CPU(1)|T_JOINABLE);
    	rt_task_start(&test[i-1], &periodicTest, (void*) i);
    }
    
   
    
    
	pthread_t disturbances[10];
    
    for (i = 0; i < 10; i++) {
    	pthread_create(&disturbances[i], NULL, disturbance, NULL);
    }
	for (i = 0; i < 10; i++) {
		pthread_join(disturbances[i], NULL);
	}

	wait_for_ctrl_c();
	
	return 0;
}

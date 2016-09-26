#include "io.h"
#include <unistd.h>
#include <pthread.h>
#include <sys/times.h>
#include <sched.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
#include  <rtdk.h>

RT_TASK taskA;
RT_TASK taskB;
RT_TASK taskC;



int set_cpu(int cpu_number)
{
	// setting cpu set to the selected cpu
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);
	// set cpu set to current thread and return
	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void* testA(void *arg){
	set_cpu(1);
	io_write(1,1);
	rt_task_set_periodic(&taskA, TM_NOW, 10000);

	while(1){
		if (io_read(1) == 0){
			io_write(1,0);
			usleep(5);
			io_write(1,1);	
		}

		rt_task_wait_period(NULL);
	}
}

void* testB(void *arg){
	set_cpu(1);
	io_write(2,1);
	rt_task_set_periodic(&taskB, TM_NOW, 10000);
	while(1){
		if (io_read(2) == 0){
			io_write(2,0);
			usleep(5);
			io_write(2,1);	
		}
		rt_task_wait_period(NULL);
	}

}


void* testC(void *arg){
	set_cpu(1);
	io_write(3,1);
	rt_task_set_periodic(&taskC, TM_NOW, 10000);
	while(1){
		if (io_read(3) == 0){
			io_write(3,0);
			usleep(5);
			io_write(3,1);	
		}
		rt_task_wait_period(NULL);
	}
}


void *busyWait(){
	int sum = 0;
	while(1){
		sum+= 1;
	}
}




int main (int argc, char **argv){
	mlockall(MCL_CURRENT|MCL_FUTURE); 
	io_init();
	rt_print_auto_init(1);

	rt_task_create (&taskA, "testA", 0, 3, T_JOINABLE);
	rt_task_create (&taskB, "testB", 0, 2, T_JOINABLE);
	rt_task_create (&taskC, "testC", 0, 1, T_JOINABLE);


	rt_task_start(&taskA, &testA, 0);	
	rt_task_start(&taskB, &testB, 0);	
	rt_task_start(&taskC, &testC, 0);

	rt_task_join(&taskA);
	rt_task_join(&taskB);
	rt_task_join(&taskC);

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

	pthread_create(&pth1, NULL, busyWait, NULL);
	pthread_create(&pth2, NULL, busyWait, NULL);
	pthread_create(&pth3, NULL, busyWait, NULL);
	pthread_create(&pth4, NULL, busyWait, NULL);
	pthread_create(&pth5, NULL, busyWait, NULL);
	pthread_create(&pth6, NULL, busyWait, NULL);
	pthread_create(&pth7, NULL, busyWait, NULL);
	pthread_create(&pth8, NULL, busyWait, NULL);
	pthread_create(&pth9, NULL, busyWait, NULL);
	pthread_create(&pth10, NULL, busyWait, NULL);

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
	return 0;
}
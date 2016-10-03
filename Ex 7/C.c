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
#include <rtdk.h>
#include <native/mutex.h>

RT_TASK taskA;
RT_TASK taskC;

RT_MUTEX mutexA;
RT_MUTEX mutexB;



int set_cpu(int cpu_number)
{
	// setting cpu set to the selected cpu
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);
	// set cpu set to current thread and return
	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void low(void *arg){
	struct rt_task_info base;
	struct rt_task_info temp;

	rt_task_inquire(NULL, &base); 
	rt_printf("Low ready! \n");	
	
	rt_mutex_acquire(&mutexA, TM_INFINITE);
	rt_task_set_priority(NULL,5);
	rt_task_sleep(1);
	rt_task_inquire(NULL, &temp); 
	rt_printf("Low: \tb:%i c:%i \n", temp.bprio, temp.cprio);
	
	busyWait(3);
	
	rt_mutex_acquire(&mutexB, TM_INFINITE);
	rt_task_inquire(NULL, &temp); 
	rt_task_sleep(1);
	rt_printf("Low: \tb:%i c:%i \n", temp.bprio, temp.cprio);
	
	busyWait(3);
	rt_mutex_release(&mutexB);
	rt_mutex_release(&mutexA);

	
	rt_task_inquire(NULL, &temp); 
	rt_printf("Low: \tb:%i c:%i \n", temp.bprio, temp.cprio);
	rt_task_set_priority(NULL,base.bprio);
	rt_task_sleep(1);
 	
 	busyWait(1);
	rt_printf("Low done! \n");
}


void high(void *arg){

	struct rt_task_info base;
	struct rt_task_info temp;
	rt_task_inquire(NULL, &base); 
	rt_task_sleep(1000 * 1000);
	rt_printf("High ready! \n");	
	
	rt_mutex_acquire(&mutexB, TM_INFINITE);
	rt_task_set_priority(NULL,4);


	rt_task_inquire(NULL, &temp); 
	rt_printf("High: \tb:%i c:%i \n", temp.bprio, temp.cprio);
	
	busyWait(1);
	
	rt_mutex_acquire(&mutexA, TM_INFINITE);
	rt_task_set_priority(NULL,5);
	
	rt_task_inquire(NULL, &temp); 
	rt_printf("High: \tb:%i c:%i \n", temp.bprio, temp.cprio);

	busyWait(2);
	
	rt_mutex_release(&mutexA);
	rt_task_set_priority(NULL,4);
	rt_task_sleep(1);
	rt_task_inquire(NULL, &temp); 
	rt_printf("High: \tb:%i c:%i \n", temp.bprio, temp.cprio);

	rt_mutex_release(&mutexB);
	rt_task_set_priority(NULL,base.bprio);
	rt_task_sleep(1);

	rt_task_inquire(NULL, &temp); 
	rt_printf("High: \tb:%i c:%i \n", temp.bprio, temp.cprio);

	busyWait(1);

	rt_printf("High done! \n");

}

void busyWait(int ticks){
	int i;
	for(i=0;i < ticks*10; i++){
		rt_timer_spin(100 * 1000);
	}
}


int main (int argc, char **argv){
	mlockall(MCL_CURRENT|MCL_FUTURE); 

	rt_print_auto_init(1);
	rt_mutex_create (&mutexA, "MutexA");
	rt_mutex_create (&mutexB, "MutexB");

	rt_task_create (&taskA, "low", 0, 1, T_JOINABLE|T_CPU(1));
	rt_task_create (&taskC, "high", 0, 2, T_JOINABLE|T_CPU(1));

	rt_task_shadow(NULL, "Main", 99, T_JOINABLE|T_CPU(1));

	rt_task_start(&taskA, &low, 0);	
	rt_task_start(&taskC, &high, 0);	
	

	rt_task_join(&taskA);
	rt_task_join(&taskC);	
	
	rt_mutex_delete (&mutexA);
	rt_mutex_delete (&mutexB);
}
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
RT_TASK taskB;
RT_TASK taskC;

RT_MUTEX mutex;


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
	set_cpu(1);
	rt_printf("Low ready! \n");	
	rt_mutex_acquire(&mutex, TM_INFINITE);
	busyWait(3);
	rt_printf("Low done! \n");
	rt_mutex_release(&mutex);

}

void med(void *arg){
	set_cpu(1);
	rt_task_sleep(1000 * 1000);
	rt_printf("Medium ready! \n");
	
	busyWait(5);
	rt_printf("Medium done! \n");

}

void high(void *arg){
	set_cpu(1);
	rt_task_sleep(2000 * 1000);
	rt_printf("High ready! \n");	
	rt_mutex_acquire(&mutex, TM_INFINITE);
	busyWait(2);
	rt_printf("High done! \n");
	
	rt_mutex_release(&mutex);

}

void busyWait(int ticks){
	int i;
	for(i=0;i < ticks*10; i++){
		rt_timer_spin(100*1000);
	}
}


int main (int argc, char **argv){
	mlockall(MCL_CURRENT|MCL_FUTURE); 

	rt_print_auto_init(1);
	rt_mutex_create (&mutex, "Mutex");

	rt_task_create (&taskA, "low", 0, 1, T_JOINABLE);
	rt_task_create (&taskB, "med", 0, 2, T_JOINABLE);
	rt_task_create (&taskC, "high", 0, 4, T_JOINABLE);

	rt_task_shadow(NULL, "Main", 99, T_JOINABLE);

	rt_task_start(&taskA, &low, 0);	
	rt_task_start(&taskB, &med, 0);	
	rt_task_start(&taskC, &high, 0);	
	



	rt_task_join(&taskA);
	rt_task_join(&taskB);	
	rt_task_join(&taskC);	
	
	rt_mutex_delete (&mutex);
}
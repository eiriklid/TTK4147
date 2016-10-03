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
#include <native/sem.h>

RT_TASK taskA;
RT_TASK taskB;

RT_SEM sem;


int set_cpu(int cpu_number)
{
	// setting cpu set to the selected cpu
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);
	// set cpu set to current thread and return
	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void testA(void *arg){
	set_cpu(1);
	rt_sem_p(&sem, TM_INFINITE);
	rt_printf("A print! \n");
	//rt_sem_v(&sem);

}

void testB(void *arg){
	set_cpu(1);
	rt_sem_p(&sem, TM_INFINITE);
	rt_printf("B print! \n");
	//rt_sem_v(&sem);

}





int main (int argc, char **argv){
	mlockall(MCL_CURRENT|MCL_FUTURE); 

	rt_print_auto_init(1);
	rt_sem_create (&sem, "SEM", 1,S_PRIO);

	rt_task_create (&taskA, "testA", 0, 1, T_JOINABLE);
	rt_task_create (&taskB, "testB", 0, 2, T_JOINABLE);
	rt_task_shadow(NULL, "Main", 99, T_JOINABLE);

	rt_task_start(&taskA, &testA, 0);	
	rt_task_start(&taskB, &testB, 0);	
	


	rt_task_sleep(2000 * 1000 * 1000);
	//sleep(1000000);
	rt_sem_broadcast(&sem); 	
	usleep(1000000);
	

	rt_task_join(&taskA);
	rt_task_join(&taskB);	
	
	rt_sem_delete (&sem);
}
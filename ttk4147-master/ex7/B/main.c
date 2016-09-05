#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sched.h>
#include <pthread.h>

#include <native/task.h>
#include <native/timer.h>
#include <native/sem.h>
#include <native/mutex.h>

#include  <rtdk.h>

#include <sys/io.h>

RT_SEM sem;
RT_SEM resourceSem;

static RT_MUTEX resourceMutex;

void busy_wait_ms(unsigned long delay){
	unsigned long count = 0;
	while (count <= delay*10){
		rt_timer_spin(1000*100);
		count++;
	}
}

int rt_task_sleep_ms(unsigned long delay){
	return rt_task_sleep(1000*1000*delay);
}

void lowFunc() {
	rt_sem_p(&sem, TM_INFINITE);
	rt_mutex_acquire(&resourceMutex, TM_INFINITE);
	rt_printf("low locks resource\n");
	busy_wait_ms(100);
	busy_wait_ms(100);
	busy_wait_ms(100);
	rt_printf("low unlocks resource\n");
	rt_mutex_release(&resourceMutex);
}

void medFunc() {
	rt_sem_p(&sem, TM_INFINITE);
	rt_task_sleep_ms(100);
	rt_printf("med starts running\n");
	busy_wait_ms(100);
	busy_wait_ms(100);
	busy_wait_ms(100);
	busy_wait_ms(100);
	busy_wait_ms(100);
	rt_printf("med finished\n");
}

void highFunc() {
	rt_sem_p(&sem, TM_INFINITE);
	rt_task_sleep_ms(200);
	rt_printf("High wants to lock resource\n");
	rt_mutex_acquire(&resourceMutex, TM_INFINITE);
	rt_printf("High locks resource\n");
	busy_wait_ms(100);
	busy_wait_ms(100);
	rt_printf("High unlocks resource\n");
	rt_mutex_release(&resourceMutex);
}

void syncFunc() {
	sleep(1);
	rt_sem_broadcast(&sem);
	sleep(1);
}

int main() {
	mlockall(MCL_CURRENT|MCL_FUTURE);
	rt_print_auto_init(1);
	
	RT_TASK low, med, high, sync;
	rt_task_create(&low, "low", 0, 10, T_CPU(1)|T_JOINABLE);
	rt_task_create(&med, "med", 0, 20, T_CPU(1)|T_JOINABLE);
	rt_task_create(&high, "high", 0, 30, T_CPU(1)|T_JOINABLE);
	rt_task_create(&sync, "sync", 0, 99, T_CPU(1)|T_JOINABLE);
	

	rt_sem_create(&sem, "sem", 0, S_PRIO);
	rt_sem_create(&resourceSem, "resourceSem", 1, S_PRIO);
	rt_mutex_create(&resourceMutex, "resourceMutex");


	rt_task_start(&low, &lowFunc, NULL);
	rt_task_start(&med, &medFunc, NULL);
	rt_task_start(&high, &highFunc, NULL);
	rt_task_start(&sync, &syncFunc, NULL);

	rt_task_join(&low);
	rt_task_join(&med);
	rt_task_join(&high);
	rt_task_join(&sync);

	rt_sem_delete(&sem);
	rt_sem_delete(&resourceSem);
	rt_mutex_delete(&resourceMutex);

	return 0;
}

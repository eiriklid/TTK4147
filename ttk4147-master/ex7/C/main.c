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

static RT_MUTEX A;
static RT_MUTEX B;
RT_TASK low, high;

#define CEILING 20

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
	rt_mutex_acquire(&A, TM_INFINITE);
	rt_task_set_priority(&low, CEILING);
	rt_printf("low locks A\n");
	busy_wait_ms(100);
	busy_wait_ms(100);
	busy_wait_ms(100);
	rt_mutex_acquire(&B, TM_INFINITE);
	rt_printf("low locks B\n");
	busy_wait_ms(100);
	busy_wait_ms(100);
	busy_wait_ms(100);
	rt_mutex_release(&B);
	rt_printf("low unlocks B\n");
	rt_mutex_release(&A);
	rt_task_set_priority(&low, 10);
	rt_printf("low unlocks A\n");
	busy_wait_ms(100);
}

void highFunc() {
	rt_sem_p(&sem, TM_INFINITE);
	rt_task_sleep_ms(100);
	rt_printf("High wants to lock B\n");
	rt_mutex_acquire(&B, TM_INFINITE);
	rt_printf("High locks B\n");
	busy_wait_ms(100);
	rt_mutex_acquire(&A, TM_INFINITE);
	busy_wait_ms(100);
	busy_wait_ms(100);
	rt_mutex_release(&A);
	rt_printf("low unlocks A\n");
	rt_mutex_release(&B);
	rt_printf("low unlocks B\n");
	busy_wait_ms(100);
}

void syncFunc() {
	sleep(1);
	rt_sem_broadcast(&sem);
	sleep(1);
}

int main() {
	mlockall(MCL_CURRENT|MCL_FUTURE);
	rt_print_auto_init(1);

	RT_TASK sync;
	
	rt_task_create(&low, "low", 0, 10, T_CPU(1)|T_JOINABLE);
	rt_task_create(&high, "high", 0, 20, T_CPU(1)|T_JOINABLE);
	rt_task_create(&sync, "sync", 0, 99, T_CPU(1)|T_JOINABLE);
	

	rt_sem_create(&sem, "sem", 0, S_PRIO);
	rt_mutex_create(&A, "A");
	rt_mutex_create(&B, "B");


	rt_task_start(&low, &lowFunc, NULL);
	rt_task_start(&high, &highFunc, NULL);
	rt_task_start(&sync, &syncFunc, NULL);

	rt_task_join(&low);
	rt_task_join(&high);
	rt_task_join(&sync);

	rt_sem_delete(&sem);
	rt_mutex_delete(&A);
	rt_mutex_delete(&B);

	return 0;
}

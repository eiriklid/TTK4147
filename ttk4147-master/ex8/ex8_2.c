#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/dispatch.h>
#include <sys/mman.h>
#include <pthread.h>


struct pid_data{
	pthread_mutex_t pid_mutex;
	pid_t pid;
};

int set_priority(int priority)
{
	int policy;
    struct sched_param param;
    // check priority in range
    if (priority < 1 || priority > 63) return -1;
    // set priority
    pthread_getschedparam(pthread_self(), &policy, &param);
    param.sched_priority = priority;
    return pthread_setschedparam(pthread_self(), policy, &param);
}

int get_priority()
{
	int policy;
    struct sched_param param;
    // get priority
    pthread_getschedparam(pthread_self(), &policy, &param);
    return param.sched_curpriority;
}

int channelID;

void send(void* i) {
	int pri = *(int*) i;

	set_priority(pri*2+50);

	char data[4];
	data[0] = 'h';
	data[1] = 'e';
	data[2] = 'i';
	data[3] = '\0';
	char dataReceive[4];
	MsgSend(channelID, data, 4, dataReceive, 4);
	printf("job done: %c%c%c\n", dataReceive[0], dataReceive[1], dataReceive[2]);
}

int main(int argc, char *argv[]) {

	set_priority(63);

	int test = shm_open("/dev/shmem2", O_RDWR, S_IRWXU);
	struct pid_data* p = (struct pid_data*) mmap(0, sizeof(struct pid_data), PROT_READ|PROT_WRITE, MAP_SHARED, test, 0);

	printf("PID IS: %d\n", p->pid);

	//OPPG 4.2
	channelID = ConnectAttach(0, p->pid, 1, 0, 0);

	pthread_t messages[4];
	int i;
    for (i = 0; i < 4; i++) {
    	pthread_create(&messages[i], NULL, &send, (void*) &i);
    }

    for (i = 0; i < 4; i++) {
    	pthread_join(messages[i], NULL);
    }
	ConnectDetach(channelID);
	return EXIT_SUCCESS;
}





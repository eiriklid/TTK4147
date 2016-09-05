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

int main(int argc, char *argv[]) {


	printf("test\n");
	set_priority(53);
	int test = shm_open("/dev/shmem2", O_RDWR | O_CREAT, S_IRWXU);
	ftruncate(test, sizeof(struct pid_data));
	struct pid_data* p;
	p = mmap(0, sizeof(struct pid_data), PROT_READ|PROT_WRITE, MAP_SHARED, test, 0);

	pthread_mutexattr_t myattr;

    pthread_mutexattr_init(&myattr);
    pthread_mutexattr_setpshared(&myattr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&p->pid_mutex, &myattr );

    p->pid = getpid();


    //OPPG 4

    char data[4];

    int channelID = ChannelCreate(0);
    struct _msg_info info;

    while(1) {
		int receiveID = MsgReceive(channelID, data, 4, &info);
		printf("Receieved: %s from process: %d, thread: %d\n", data, info.pid, info.tid);
		printf("Server priority: %d\n", get_priority());
		char first = data[0];
		data[0] = data[2];
		data[2] = first;
		data[3] = '\0';
		MsgReply(receiveID, 0, data, 4);
    }

	return EXIT_SUCCESS;
}



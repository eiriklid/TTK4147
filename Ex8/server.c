#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/dispatch.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sys/neutrino.h>

struct pid_data{
	pthread_mutex_t pid_mutex;
	pid_t pid;
};

void create_shared_memory(){

	int shm = shm_open("/sharedpid", O_RDWR | O_CREAT, S_IRWXU);

	ftruncate(shm, sizeof(struct pid_data));

	struct pid_data *map = mmap(0, sizeof(struct pid_data), PROT_READ|PROT_WRITE, MAP_SHARED, shm, 0);

	pthread_mutexattr_t myattr;



    pthread_mutexattr_init(&myattr);
    pthread_mutexattr_setpshared(&myattr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&map->pid_mutex, &myattr );

    map->pid = getpid();

}

void message_server(){
	int channel_id = ChannelCreate(/*_NTO_CHF_FIXED_PRIORITY*/0);
	char data[256];
	char reply[256];
	set_priority(3);
	sprintf(reply, "From server");
	struct _msg_info info;
	while(1){
		printf("Priority before receive: %d\n", get_priority());
		int receive_id = MsgReceive(channel_id, data, sizeof(data), &info);
		printf("Received %s, from id: %d\n", data,(int) info.tid);
		printf("Priority after receive: %d\n", get_priority());
		printf("\n");
		MsgReply(receive_id, EOK, reply, sizeof(reply));
	}
}

int set_priority(int priority){
     int policy;
     struct sched_param param;
     // check priority in range
     if (priority < 1 || priority > 63) return -1;
     // set priority
     pthread_getschedparam(pthread_self(), &policy, &param);
     param.sched_priority = priority;
     return pthread_setschedparam(pthread_self(), policy, &param);
}

int get_priority(){
     int policy;
     struct sched_param param;
     // get priority
     pthread_getschedparam(pthread_self(), &policy, &param);
     return param.sched_curpriority;
}


int main(int argc, char *argv[]) {
	create_shared_memory();

    printf("Write process pid %d\n", getpid());
    message_server();
    /*
    while(1){
    	printf("Loop\n");

    }
*/
	return EXIT_SUCCESS;
}

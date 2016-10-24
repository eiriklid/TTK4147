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

void client1(int server_id){
	char send[256];
	char receive[256];
	set_priority(1);
	int channel_id = ConnectAttach(0, server_id, 1, 0, 0);
	sprintf(send, "Sending from client");
	int status_value = MsgSend(channel_id, send, sizeof(send), receive, sizeof(receive));
	ConnectDetach(channel_id);
	printf("Status value %d\n", status_value);
	printf("Server message %s\n", receive);
}
void client2(int server_id){
	char send[256];
	char receive[256];
	set_priority(2);
	int channel_id = ConnectAttach(0, server_id, 1, 0, 0);
	sprintf(send, "Sending from client");
	int status_value = MsgSend(channel_id, send, sizeof(send), receive, sizeof(receive));
	ConnectDetach(channel_id);
	printf("Status value %d\n", status_value);
	printf("Server message %s\n", receive);
}
void client3(int server_id){
	char send[256];
	char receive[256];
	set_priority(4);
	int channel_id = ConnectAttach(0, server_id, 1, 0, 0);
	sprintf(send, "Sending from client");
	int status_value = MsgSend(channel_id, send, sizeof(send), receive, sizeof(receive));
	ConnectDetach(channel_id);
	printf("Status value %d\n", status_value);
	printf("Server message %s\n", receive);
}
void client4(int server_id){
	char send[256];
	char receive[256];
	set_priority(5);
	int channel_id = ConnectAttach(0, server_id, 1, 0, 0);
	sprintf(send, "Sending from client");
	int status_value = MsgSend(channel_id, send, sizeof(send), receive, sizeof(receive));
	ConnectDetach(channel_id);
	printf("Status value %d\n", status_value);
	printf("Server message %s\n", receive);
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

void create_main(){
	int shm  = shm_open("/sharedpid", O_RDWR, S_IRWXU);

	struct pid_data *map = mmap(0, sizeof(struct pid_data), PROT_READ|PROT_WRITE, MAP_SHARED, shm, 0);

    printf( "Shared memory pid %d\n", map->pid);
    printf("Read process pid %d\n", getpid());

    set_priority(10);

	pthread_t pth1;
	pthread_t pth2;
	pthread_t pth3;
	pthread_t pth4;

	pthread_create(&pth1, NULL, (void*)client1, (void*)map->pid);
	pthread_create(&pth2, NULL, (void*)client2, (void*)map->pid);
	pthread_create(&pth3, NULL, (void*)client3, (void*)map->pid);
	pthread_create(&pth4, NULL, (void*)client4, (void*)map->pid);

	pthread_join(pth1, NULL);
	pthread_join(pth2, NULL);
	pthread_join(pth3, NULL);
	pthread_join(pth4, NULL);
}

int main(int argc, char *argv[]) {
	pthread_t pth;

	pthread_create(&pth, NULL, (void*)create_main,NULL);
	pthread_join(pth,NULL);



	//message_client(map->pid);
	return EXIT_SUCCESS;
}

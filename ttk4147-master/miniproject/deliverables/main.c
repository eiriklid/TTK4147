#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "udp.h"
#include "channel.h"

#define TO_UDP 0
#define TO_CONTROLLER 1
#define TO_RESPONDER 2

#define REFERENCE 1
#define K_p 10
#define K_i 800
#define PERIOD_MS 4
#define RUNNING_TIME 0.5

#define MS_TO_S(ms) (ms * 0.001)
#define PERIOD MS_TO_S(PERIOD_MS)

#define MS_TO_US(ms) (ms * 1000)
#define PERIOD_US MS_TO_US(PERIOD_MS)

#define ITERATIONS_COUNT (RUNNING_TIME / PERIOD)

static pthread_t udpsend;
static pthread_t udpreceive;
static pthread_t controller;
static pthread_t responder;

static struct udp_conn conn;

static int iterations = 0;



void* udpSend(void* channelsParameter) {
	struct channel* channels = (struct channel*) channelsParameter;
	
	udp_send(&conn, "START", 6);
	channel_put(&channels[TO_CONTROLLER], "START");
	
	char toSend[128];
	while(iterations < ITERATIONS_COUNT) {
		channel_get(&channels[TO_UDP], toSend);

		if (strncmp(toSend, "GET", 3) == 0) {
			udp_send(&conn, toSend, 4);	
		} else if (strncmp(toSend, "SET", 3) == 0) {
			udp_send(&conn, toSend, strlen(toSend)+1);
		} else if (strncmp(toSend, "SIGNAL_ACK", 10) == 0) {
			udp_send(&conn, toSend, strlen(toSend)+1);
		}
	}
	udp_send(&conn, "STOP", 5);
	return NULL;
}



void* udpReceive(void* channelsParameter) {
	struct channel* channels = (struct channel*) channelsParameter;

	char receive[128];

	while(iterations < ITERATIONS_COUNT-1) {
		udp_receive(&conn, receive, 128);

		if (strncmp(receive, "GET_ACK", 7) == 0) {
			channel_put(&channels[TO_CONTROLLER], receive);
		} else if (strncmp(receive, "SIGNAL", 6) == 0) {
			channel_put(&channels[TO_RESPONDER], receive);
		}
	}
	return NULL;
}



void* responderFunc(void* channelsParameter) {
	struct channel* channels = (struct channel*) channelsParameter;

	char receive[128];

	while(iterations < ITERATIONS_COUNT-1) {
		channel_get(&channels[TO_RESPONDER], receive);
		if (strncmp(receive, "SIGNAL", 6) == 0) {
			channel_put(&channels[TO_UDP], "SIGNAL_ACK");
		}
	}
	return NULL;
}



void* controllerFunc(void* channelsParameter) {
	struct channel* channels = (struct channel*) channelsParameter;
	char receive[128], send[128];
	char* command;
	double y, u, integral = 0;

	channel_get(&channels[TO_CONTROLLER], receive);

	struct timespec next;
	clock_gettime(CLOCK_REALTIME, &next);

	while(iterations < ITERATIONS_COUNT) {
		channel_put(&channels[TO_UDP], "GET");
		
		channel_get(&channels[TO_CONTROLLER], receive);
		command = strtok(receive, ":");
		y = atof(strtok(NULL, ":"));

		integral += ( (REFERENCE - y) * PERIOD);
		u = K_p * (REFERENCE - y) + K_i * integral;
		
		sprintf(send, "SET:%3.3f", u);
		channel_put(&channels[TO_UDP], send);

		timespec_add_us(&next, PERIOD_US);
		clock_nanosleep(&next);
		iterations++;
	}
	return NULL;
}



int main()
{
	struct channel channels[3];
	
	for (int i = 0; i < 3; i ++) {
		channel_init(&channels[i]);
	}

	udp_init_client(&conn, 9999, "192.168.0.1");

	pthread_create(&udpsend, NULL, udpSend, (void*) channels);
	pthread_create(&udpreceive, NULL, udpReceive, (void*) channels);
	pthread_create(&controller, NULL, controllerFunc, (void*) channels);
	pthread_create(&responder, NULL, responderFunc, (void*) channels);
	pthread_join(udpsend, NULL);
	pthread_join(udpreceive, NULL);
	pthread_join(controller, NULL);
	pthread_join(responder, NULL);

	return 0;
}


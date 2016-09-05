#ifndef CHANNEL_H
#define CHANNEL_H

#include <semaphore.h>

struct channel {
	char buffer[10][20];
	int head;
	int tail;
	sem_t sem;
};

void channel_init(struct channel* channel);
void channel_put(struct channel* channel, char* elem);
void channel_get(struct channel* channel, char* buf);

#endif

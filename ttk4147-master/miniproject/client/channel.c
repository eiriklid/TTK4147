#include "channel.h"

#include <string.h>

void channel_init(struct channel* channel) {
	channel->head = 0;
	channel->tail = 0;
	sem_init(&channel->sem, 0, 0);
}

void channel_put(struct channel* channel, char* elem) {
	strcpy(channel->buffer[channel->tail], elem);
	channel->tail = (channel->tail + 1) % 10;
	sem_post(&channel->sem); 
}

void channel_get(struct channel* channel, char* buf) {
	sem_wait(&channel->sem);
	strcpy(buf, channel->buffer[channel->head]);
	channel->head = (channel->head + 1) % 10;
}

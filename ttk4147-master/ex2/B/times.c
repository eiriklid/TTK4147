#include <sys/times.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int GLOBAL = 0;

void* ink(void* lokal){
	GLOBAL++;
	(*( (int*) lokal))++;
	printf("Global: %i, Lokal: %i \n", GLOBAL, (*( (int*) lokal)));
	return NULL;
}


int main() {

	int lokal = 0;

    pthread_t one;
    pthread_t two;

    pthread_create(&one, NULL, ink, (void*) &lokal);
    pthread_create(&two, NULL, ink, (void*) &lokal);

    pthread_join(one, NULL);
    pthread_join(two, NULL);

    return 0;
}

//Både den lokale i main og den globale variabelen er delt av trådene. Derfor blir de inkrementert to ganger


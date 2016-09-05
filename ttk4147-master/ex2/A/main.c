#include <unistd.h>
#include <stdio.h>

int GLOBAL = 0;

int main() {
	pid_t fork(void);
	int lokal = 0;
	
	pid_t a = vfork();
	if (a == 0) {
		//Child process
		GLOBAL++;
		lokal++;
		printf ("Child process: %i, %i \n", GLOBAL, lokal);
		_exit(0);
	}
	else if (a > 0) {
		//Parent process
		GLOBAL++;
		lokal++;
		printf ("Parent process: %i, %i \n", GLOBAL, lokal);
	}
	else {
		printf ("error");
	}



	return 0;
}


//vfork lager en tråd, og burker minneadressen til parent
//må bruke exit() for å avslutte childprocess, da blir det rett :D

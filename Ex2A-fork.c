#include<unistd.h>
#include<stdio.h>

int global = 0;


int main(){
	int local = 0;

	pid_t pid = fork();
	if (pid == 0)
	{
		//child
		global +=1;
		local +=1;
		printf("child process global: %i\n", global );
		printf("child process local: %i\n", local);

	}
	else if (pid > 0)
	{
		//parent
		global +=1;
		local +=1;
		printf("parent process global: %i\n", global );
		printf("parent process local: %i\n", local);
	}
	else {
		//FAIL
		printf("Error\n");
	}


}
/* Proceso en el que el padre detiene al hijo con un señal SIGSTOP */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>


int main()
{
	pid_t pid;
	
	pid = fork();
	if (pid == 0){
		printf("\nSoy un hijo muy vago, voy echarme la siesta\n");
		sleep(10);
		printf("\nAy de mi, soy el hijo, muero!!!");
	}
	else{
		printf("\nEn 5 segundos voy a congelar a mi hijo %d\n",pid);
		sleep(5);
		kill(pid, SIGSTOP);
		wait(NULL);  // Espera a que el hijo termine
	}

	return 0;
}

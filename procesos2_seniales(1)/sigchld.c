// Ejemplo de proceso padre esperando por la terminación de un hijo

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

void handler(int sig)
{
	pid_t pid;
	pid = wait(NULL);
	printf("Pid %d murio.Ya me estaba rayando esperar tanto\n", pid);
}
int main(void)
{
signal(SIGCHLD, handler);
if(!fork())
	{
	printf("Soy el hijo, con pid %d y mi padre es %d\n", getpid(),getppid());
	sleep(60);
	exit(0);
	}
printf("Soy el padre %d. Aqui estoy, esperando a que el chaval termine...\n",getpid());
getchar();
return 0;
}

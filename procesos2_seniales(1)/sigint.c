#include<stdio.h>
#include<signal.h>
#include<unistd.h>

void sig_handler(int signo)
{
  if (signo == SIGINT)
    printf("SIGINT no me hace pupita\n");
}

int main(void)
{
  if (signal(SIGINT, sig_handler) == SIG_ERR)
	printf("\nError al intentar capturar SIGINT\n");
  printf("\nSoy inmune a CTRL+C, ja, ja, ja ...\n");
  // 
  while(1) 
    sleep(1);
  return 0;
}

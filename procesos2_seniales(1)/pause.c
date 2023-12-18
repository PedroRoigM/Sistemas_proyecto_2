/* Programa que programa una alarma de 60 segundos y queda en pause().
Si no hay una señal antes, vuelve al inicio */


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

static void catchSignal(int signo);

int main(void){


    printf("\nMe voy a echar una siestecita de 60 segundos\n");
    alarm(60);

    signal(SIGALRM, catchSignal);
   
    pause();

    printf("We should not see this text.\n");

    return 1;
}

static void catchSignal(int signo){
    printf("Caught asignal.\n");
    printf("Signal value = %d\n", signo);

    printf("Exiting from process...\n");

    //in stdlib.h header file
    exit(EXIT_SUCCESS);

}

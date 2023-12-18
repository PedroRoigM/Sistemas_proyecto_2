/* Programa para mostrar el uso de SIGALRM para construir un
 temporizador. Mientras no ocurra nada especial, el proceso
 sigue en un bucle infinito que muestra un mensaje cada segundo.
 
 Este proceso también nos sirve para ver la utilidad de SIGSTOP y
 SIGCONT. Con SIGSTOP queda bloqueado
 
 > kill -s SIGSTOP pid
 
 Puede verse con el comando top, aparecerá un proceso detenido. Los
 depuradores usan esta señal.
 
 Con SIGCONT vuelve a la ejecución normal
 
 > kill -s SIGCONT pid
 
 */

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

static void alarmHandler(int signo);

int main(void){
	
	int i;

    alarm(5);

    signal(SIGALRM, alarmHandler);

    for(i = 1; i < 1000; i++){
        printf("%d\n", i);
        sleep(1);    
    }

    return 0;

}

static void alarmHandler(int signo){
    printf("Alarm signal sent!\n");

}


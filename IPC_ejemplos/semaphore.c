// Adaptado de https://www.geeksforgeeks.org/use-posix-semaphores-c/
// C program to demonstrate working of Semaphores

// Compilar con gcc semaphore.c -lpthread -lrt -o semaphore
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
  
sem_t mutex;
  
void* thread(void* arg)
{
    //wait
    sem_wait(&mutex);
    printf("\nEntrando en la seccion critica..\n");
    printf("\nManda narices, cuatro segundos de siesta\n");
    //critical section
    sleep(4);
      
    //signal
    printf("\nSaliendo de la seccion critica...\n");
    sem_post(&mutex);
}
  
  
int main()
{
    sem_init(&mutex, 0, 1);
    pthread_t t1,t2;
    pthread_create(&t1,NULL,thread,NULL);
    pthread_create(&t2,NULL,thread,NULL);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    sem_destroy(&mutex);
    return 0;
}

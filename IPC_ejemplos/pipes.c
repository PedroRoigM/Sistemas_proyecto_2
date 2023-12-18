// C program to demonstrate use of fork() and pipe()

// Adaptado de https://www.geeksforgeeks.org/c-program-demonstrate-fork-and-pipe/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>
#include<ctype.h>

#define LEN_MESG 100
  
int main()
{
    // We use two pipes
    // First pipe to send input string from parent
    // Second pipe to send concatenated string from child
  
    int fd1[2];  // Used to store two ends of first pipe
    int fd2[2];  // Used to store two ends of second pipe
    int i;
    char input_str[LEN_MESG];
    pid_t p;
  
    if (pipe(fd1)==-1)
    {
        fprintf(stderr, "Pipe Failed" );
        return 1;
    }
    if (pipe(fd2)==-1)
    {
        fprintf(stderr, "Pipe Failed" );
        return 1;
    }
  
    printf("Dime algo: ");
    
    fgets(input_str,LEN_MESG,stdin);
    p = fork();
  
    if (p < 0)
    {
        fprintf(stderr, "fork Failed" );
        return 1;
    }
  
    // Parent process
    else if (p > 0)
    {
        char concat_str[LEN_MESG];
        
  
        close(fd1[0]);  // Close reading end of first pipe
  
        // Write input string and close writing end of first
        // pipe.
        write(fd1[1], input_str, strlen(input_str)+1);
        close(fd1[1]);
  
        // Wait for child to send a string
        wait(NULL);
  
        close(fd2[1]); // Close writing end of second pipe
  
        // Read string from child, print it and close
        // reading end.
        read(fd2[0], concat_str, LEN_MESG);
        printf("SOY EL PADRE Y LA RESPUESTA DEL HIJO ES: %s\n", concat_str);
        close(fd2[0]);
    }
  
    // child process
    else
    {
        close(fd1[1]);  // Close writing end of first pipe
  
        // Read a string using first pipe
        char concat_str[LEN_MESG];
        read(fd1[0], concat_str, LEN_MESG);
        printf("concat_str %s \n",concat_str);
  

        // Close both reading ends
        close(fd1[0]);
        close(fd2[0]);
  
        printf("SOY EL HIJO Y HE LEIDO %s\n",concat_str);

        // Convert to UPPERCASE
        for (i=0; i<strlen(concat_str); i++)
			concat_str[i] = toupper(concat_str[i]);

        // Write concatenated string and close writing end
        write(fd2[1], concat_str, strlen(concat_str)+1);
        close(fd2[1]);
  
        exit(0);
    }
}

// C Program for Message Queue (Writer Process)
//
// Utiliza la interfaz System V
//    
// Adaptado de https://www.geeksforgeeks.org/ipc-using-message-queues
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
  
#define LONGITUD_MSG 100
// structure for message queue
struct mesg_buffer {
    long mesg_type;
    char mesg_text[LONGITUD_MSG];
} message;
  
int main()
{
    key_t key;
    int msgid;
  
    //ftok to generate unique key
    key = ftok("progfile", 65);
  
    // msgget creates a message queue
    // and returns identifier
    msgid = msgget(key, 0666 | IPC_CREAT);
    message.mesg_type = 1;
  
    printf("Write Data : ");
    fgets(message.mesg_text,LONGITUD_MSG,stdin);
 
    // msgsnd to send message
    msgsnd(msgid, &message, sizeof(message), 0);
    return 0;
}

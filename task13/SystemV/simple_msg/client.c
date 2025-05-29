#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#define MSGLEN 10

struct message {
    long mtype;
    char mtext[MSGLEN];
};

int main() {
    struct message msg;
    struct message receive_msg;
    key_t key = ftok(".", 'A');
    int mqid = msgget(key, 0600);

    msg.mtype = 5;
    strcpy(msg.mtext, "Hello!\n");
    msgrcv(mqid, &receive_msg, sizeof(receive_msg.mtext), 10, 0);
    printf("msg with prio %ld received: %s", receive_msg.mtype, receive_msg.mtext);
    msgsnd(mqid, &msg, sizeof(msg.mtext), 0);
    return 0;
}
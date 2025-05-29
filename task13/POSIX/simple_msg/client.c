#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string.h>
#define MQNAME "/mq"
#define MSGLEN 10
int main() {
    unsigned prio;
    char msg[MSGLEN] = "Hello!\n";
    char receive_msg[MSGLEN];
    mqd_t mq = mq_open(MQNAME, O_RDWR, 0600, NULL);
    if (mq == (mqd_t)-1) {
        perror("mq_open (client)");
        return 1;
    }
    mq_receive(mq, receive_msg, MSGLEN, &prio);
    printf("msg with prio %d received: %s", prio, receive_msg);
    prio = 5;
    mq_send(mq, msg, strlen(msg)+1, prio);
    mq_close(mq);
    return 0;
}
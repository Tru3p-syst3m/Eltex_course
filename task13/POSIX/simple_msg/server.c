#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#define MQNAME "/mq"
#define MSGLEN 10
int main() {
    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = MSGLEN,
        .mq_curmsgs = 0
    };
    unsigned prio = 10;
    char msg[MSGLEN] = "Hi!\n";
    char receive_msg[MSGLEN];
    mqd_t mq = mq_open(MQNAME, O_RDWR|O_CREAT, 0600, &attr);
    mq_send(mq, msg, strlen(msg)+1, prio);
    sleep(10);                                  /*Execute client while sleeping*/
    mq_receive(mq, receive_msg, MSGLEN, &prio);
    printf("msg with prio %d received: %s", prio, receive_msg);
    mq_close(mq);
    mq_unlink(MQNAME);
    return 0;
}
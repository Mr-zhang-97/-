#include <mqueue.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>
#include "logOutput.h"

#define MAX_QUEUE_SIZE 10
#define MAX_QUEMSG_NUM 10
#define MSG_QUEUE_CLOCK	"/msg_queue_clock"
mqd_t gloable_msgQue_clock = 0;

typedef enum _MSG_QUEUE_TYPE{
	clock_queue = 0
}msg_queu_type;
//#define queueType_check()	assert()  //how to check enum type?

int open_msgQue(msg_queu_type queType);

int unlink_msgQue(msg_queu_type queTyep);

int send_msgQue(mqd_t mq, const char *msg_ptr, size_t msg_len);

int recv_msgQue(mqd_t mq, char *msg_ptr, size_t msg_len);

int create_thread(pthread_t* pid, void *(*start_fun)(void *), void *arg);

int join_thread(pthread_t pid, int *retval);


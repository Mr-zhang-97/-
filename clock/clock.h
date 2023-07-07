#ifndef _CLOCK_H_
#define _CLOCK_H_
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
#include "getTime.h"
#include <unistd.h>

#define MAX_QUEUEMSG_LEN 10
#define MAX_QUEMSG_NUM 10
#define MSG_QUEUE_CLOCK	"/msg_queue_clock"
#define __USE_MISC 1

typedef enum _MSG_QUEUE_TYPE{
	clock_queue = 0
}msg_queu_type;
//#define queueType_check()	assert()  //how to check enum type?

//record a piece of clock time;
typedef struct _CLOCK_TIME
{
  int tm_sec;			/* Seconds.	[0-60] (1 leap second) */
  int tm_min;			/* Minutes.	[0-59] */
  int tm_hour;			/* Hours.	[0-23] */
  int tm_mday;			/* Day.		[1-31] */
  int tm_mon;			/* Month.	[0-11] */
  int tm_year;			/* Year	- 1900.  */
  int tm_wday;			/* Day of week.	[0-6] */
  int tm_yday;			/* Days in year.[0-365]	*/
  int tm_isdst;			/* DST.		[-1/0/1]*/

# ifdef	__USE_MISC
  long int tm_gmtoff;		/* Seconds east of UTC.  */
  const char *tm_zone;		/* Timezone abbreviation.  */
# else
  long int __tm_gmtoff;		/* Seconds east of UTC.  */
  const char *__tm_zone;	/* Timezone abbreviation.  */
# endif
}clock_time;

//list for cloack schedule;
typedef struct __CLOCK_LIST
{
	clock_time clock_time;
	struct __CLOCK_LIST* pnext;
	int size;
}clock_list;

#define MAX_SCHEDULE_NUM 10
clock_list today_schedule;
clock_time all_schedul[MAX_SCHEDULE_NUM];

#define LOCK(mutexLock)		pthread_mutex_lock(&mutexLock)
#define UNLOCK(mutexLock)	pthread_mutex_lock(&mutexLock)
#define DESTORY_LOCK(mutexLock)	pthread_mutex_destroy(&mutexLock)
#define INIT_LOCK(mutexLock)	pthread_mutex_init(&mutexLock, NULL)

int open_msgQue(msg_queu_type queType);

int unlink_msgQue(msg_queu_type queTyep);

int send_msgQue(mqd_t mq, const char *msg_ptr, size_t msg_len);

int recv_msgQue(mqd_t mq, char *msg_ptr, size_t msg_len);

int create_thread(pthread_t* pid, void *(*start_fun)(void *), void *arg);

int join_thread(pthread_t pid, int *retval);

clock_list *listNode_create(clock_time *pclockTime);

int listNode_free(clock_list *phead);

int listNode_add(clock_list *phead, clock_time *pclockTime);

int listNode_add_inOrder(clock_list *phead, clock_time *pclockTime);

int clockTask_clear();

int clockTask_add(clock_time __clockTime);

int start_clock_task();

#endif

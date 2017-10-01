#ifndef __TASK_DEBUG_MSG_H__
#define __TASK_DEBUG_MSG_H__

#include "../ak/message.h"

extern pthread_cond_t cv;
extern pthread_mutex_t mt_cv;

extern q_msg_t gw_task_debug_msg_mailbox;
extern void* gw_task_debug_msg_entry(void*);

#endif //__TASK_DEBUG_MSG_H__

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ak.h"
#include "timer.h"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"
#include "app_data.h"

#include "task_list.h"
#include "task_list_if.h"
#include "task_sm.h"

q_msg_t gw_task_sm_mailbox;

void* gw_task_sm_entry(void*) {
	wait_all_tasks_started();

	APP_DBG("[STARTED] gw_task_sm_entry\n");

	ak_msg_t* msg;

	while (1) {
		/* get messge */
		msg = msg_get(GW_TASK_SM_ID);

		/* free message */
		msg_free(msg);
	}

	return (void*)0;
}

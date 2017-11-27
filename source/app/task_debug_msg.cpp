#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app.h"
#include "app_if.h"
#include "app_data.h"
#include "app_dbg.h"

#include "task_list.h"
#include "task_list_if.h"
#include "if_rf24.h"

#include "task_debug_msg.h"

q_msg_t gw_task_debug_msg_mailbox;

void* gw_task_debug_msg_entry(void*) {
	task_mask_started();
	wait_all_tasks_started();

	APP_DBG("[STARTED] gw_task_debug_msg_entry\n");

	while (1) {

		while (msg_available(GW_TASK_DEBUG_MSG_ID)) {
			/* get messge */
			ak_msg_t* msg = rev_msg(GW_TASK_DEBUG_MSG_ID);

			switch (msg->header->sig) {

			default:
				break;
			}

			/* free message */
			free_msg(msg);
		}
	}

	return (void*)0;
}

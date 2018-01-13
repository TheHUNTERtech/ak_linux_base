#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <semaphore.h>

#include "ak.h"

#include "sys_dbg.h"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"
#include "app_data.h"

#include "task_list.h"
#include "task_list_if.h"
#include "if_cpu_serial.h"
#include "link_sig.h"

q_msg_t gw_task_if_cpu_serial_mailbox;

void* gw_task_if_cpu_serial_entry(void*) {
	task_mask_started();
	wait_all_tasks_started();

	APP_DBG("[STARTED] gw_task_if_cpu_serial_entry\n");

	while (1) {
		while (msg_available(GW_TASK_IF_CPU_SERIAL_ID)) {
			/* get messge */
			ak_msg_t* msg = rev_msg(GW_TASK_IF_CPU_SERIAL_ID);

			switch (msg->header->sig) {
			case GW_CPU_SERIAL_IF_PURE_MSG_OUT: {
				msg_inc_ref_count(msg);
				set_msg_sig(msg, GW_LINK_SEND_PURE_MSG);
				task_post(GW_LINK_ID, msg);
			}
				break;

			case GW_CPU_SERIAL_IF_COMMON_MSG_OUT: {
				msg_inc_ref_count(msg);
				set_msg_sig(msg, GW_LINK_SEND_COMMON_MSG);
				task_post(GW_LINK_ID, msg);
			}
				break;

			case GW_CPU_SERIAL_IF_DYNAMIC_MSG_OUT: {
				msg_inc_ref_count(msg);
				set_msg_sig(msg, GW_LINK_SEND_DYNAMIC_MSG);
				task_post(GW_LINK_ID, msg);
			}
				break;

			case GW_CPU_SERIAL_IF_PURE_MSG_IN: {
				msg_inc_ref_count(msg);
				set_msg_sig(msg, GW_IF_PURE_MSG_IN);
				task_post(GW_TASK_IF_ID, msg);
			}
				break;

			case GW_CPU_SERIAL_IF_COMMON_MSG_IN: {
				msg_inc_ref_count(msg);
				set_msg_sig(msg, GW_IF_COMMON_MSG_IN);
				task_post(GW_TASK_IF_ID, msg);
			}
				break;

			case GW_CPU_SERIAL_IF_DYNAMIC_MSG_IN: {
				msg_inc_ref_count(msg);
				set_msg_sig(msg, GW_IF_DYNAMIC_MSG_IN);
				task_post(GW_TASK_IF_ID, msg);
			}
				break;

			default:
				break;
			}

			/* free message */
			free_msg(msg);
		}
	}

	return (void*)0;
}

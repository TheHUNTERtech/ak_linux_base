#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>

#include "app.h"
#include "app_data.h"
#include "app_dbg.h"

#include "task_list.h"
#include "task_if.h"
#include "task_snmp.h"

using namespace std;

q_msg_t gw_task_snmp_mailbox;

void* gw_task_snmp_entry(void*) {
	wait_all_tasks_started();
	APP_DBG("[STARTED] gw_task_snmp_entry\n");

	ak_msg_t* msg;

	while (1) {
		/* get messge */
		msg = msg_get(GW_TASK_SNMP_ID);

		/* handler message */
		switch (msg->header->sig) {

		default:
			break;
		}

		/* free message */
		msg_free(msg);
	}

	return (void*)0;
}

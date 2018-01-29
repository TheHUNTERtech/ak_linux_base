#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <ctime>
#include <string>

#include "ak.h"
#include "timer.h"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"
#include "app_data.h"

#include "task_list.h"
#include "task_list_if.h"
#include "task_cloud.h"

using namespace std;

q_msg_t gw_task_cloud_mailbox;

void* gw_task_cloud_entry(void*) {
	ak_msg_t* msg = AK_MSG_NULL;

	wait_all_tasks_started();

	APP_DBG("[STARTED] gw_task_cloud_entry\n");

	while (1) {
			/* get messge */
			msg = ak_msg_rev(GW_TASK_CLOUD_ID);

			/* free message */
			ak_msg_free(msg);
	}

	return (void*)0;
}

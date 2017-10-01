#ifndef __TASK_LIST_H__
#define __TASK_LIST_H__

#include "../ak/ak.h"
#include "../ak/message.h"

#include "app_data.h"

#include "if_console.h"
#include "if_rf24.h"
#include "if_rf433.h"
#include "if_app.h"
#include "if_cpu_serial.h"

#include "task_console.h"
#include "task_snmp.h"
#include "task_cloud.h"
#include "task_if.h"
#include "task_debug_msg.h"
#include "task_sm.h"
#include "task_sensor.h"
#include "task_sys.h"
#include "task_pub_sub.h"
#include "task_fw.h"

/** default if_des_type when get pool memory
 * this define MUST BE coresponding with app.
 */
#define AK_APP_TYPE_IF						IF_TYPE_APP_GW

/* task list MUST BE increase order */
#define AK_TASK_TIMER_ID					0
#define GW_TASK_IF_CONSOLE_ID				1
#define GW_TASK_CONSOLE_ID					2
#define GW_TASK_SNMP_ID						3
#define GW_TASK_CLOUD_ID					4
#define GW_TASK_IF_ID						5
#define GW_TASK_DEBUG_MSG_ID				6
#define GW_TASK_IF_APP_ID					7
#define GW_TASK_IF_CPU_SERIAL_ID			8
#define GW_TASK_SM_ID						9
#define GW_TASK_SENSOR_ID					10
#define GW_TASK_SYS_ID						11
#define GW_TASK_PUB_SUB_ID					12
#define GW_TASK_FW_ID						13

/* size of task list table */
#define AK_TASK_LIST_LEN					14

extern ak_task_t task_list[];

#endif //__TASK_LIST_H__

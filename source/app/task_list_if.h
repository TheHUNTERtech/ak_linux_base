#ifndef __TASK_LIST_IF_H__
#define __TASK_LIST_IF_H__

enum {
	/* SYSTEM TASKS */
	TASK_TIMER_TICK_ID,

	/* APP TASKS */
	AC_TASK_FW_ID,
	AC_TASK_SHELL_ID,
	AC_TASK_LIFE_ID,
	AC_TASK_IF_ID,
	AC_TASK_RF24_IF_ID,
	AC_TASK_UART_IF_ID,
	AC_TASK_DBG_ID,
	AC_TASK_DISPLAY_ID,

	/* NRF24 NETWORKS */
	AC_RF24_PHY_ID,
	AC_RF24_MAC_ID,
	AC_RF24_NWK_ID,

	/* LINK */
	AC_LINK_PHY_ID,
	AC_LINK_MAC_ID,
	AC_LINK_ID,

	/* EOT task ID */
	AK_TASK_EOT_ID,
};

#endif //__TASK_LIST_IF_H__

/**
 ******************************************************************************
 * @author: ThanNT
 * @date:   13/08/2016
 ******************************************************************************
**/

#ifndef __APP_IF_H__
#define __APP_IF_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************/
/* FIRMWARE task define
 */
/*****************************************************************************/
/* define timer */
#define FW_PACKED_TIMEOUT_INTERVAL			(5000)
#define FW_CHECKING_INTERVAL				(500)

/* define signal */
#define FW_CRENT_APP_FW_INFO_REQ			(1)
#define FW_CRENT_BOOT_FW_INFO_REQ			(2)
#define FW_UPDATE_REQ						(3)
#define FW_UPDATE_SM_OK						(4)
#define FW_TRANSFER_REQ						(5)
#define FW_INTERNAL_UPDATE_APP_RES_OK		(6)
#define FW_INTERNAL_UPDATE_BOOT_RES_OK		(7)
#define FW_SAFE_MODE_RES_OK					(8)
#define FW_UPDATE_SM_BUSY					(9)
#define FW_PACKED_TIMEOUT					(10)
#define FW_CHECKING_REQ						(11)

/*****************************************************************************/
/*  LIFE task define
 */
/*****************************************************************************/
/* define timer */
#define AC_LIFE_TASK_TIMER_LED_LIFE_INTERVAL		(1000)

/* define signal */
#define AC_LIFE_SYSTEM_CHECK						(0)

/*****************************************************************************/
/*  SHELL task define
 */
/*****************************************************************************/
/* define timer */

/* define signal */
#define AC_SHELL_LOGIN_CMD							(0)
#define AC_SHELL_REMOTE_CMD							(1)

/*****************************************************************************/
/*  RF24 task define
 */
/*****************************************************************************/
/* define timer */
#define AC_RF24_IF_POLLING_INTERVAL					(100)	/* 100 ms */
#define AC_RF24_IF_TIMER_PACKET_DELAY_INTERVAL		(100)	/* 100 ms */

/* interrupt signal */
#define AC_RF24_IF_IRQ_TX_FAIL						(1)
#define AC_RF24_IF_IRQ_TX_SUCCESS					(2)
#define AC_RF24_IF_IRQ_RX_READY						(3)

/* define signal */
#define AC_RF24_IF_INIT_NETWORK						(4)
#define AC_RF24_IF_PURE_MSG_OUT						(5)
#define AC_RF24_IF_COMMON_MSG_OUT					(6)
#define AC_RF24_IF_TIMER_PACKET_DELAY				(7)

/* private define */
#define AC_RF24_IF_WATCH_DOG_COUNTER_MAX			30		/* 30' */

/*****************************************************************************/
/* IF task define
 */
/*****************************************************************************/
/* define timer */
#define AC_IF_TIMER_PACKET_TIMEOUT_INTERVAL			(500)

/* define signal */
#define AC_IF_PURE_MSG_IN							(1)
#define AC_IF_PURE_MSG_OUT							(2)
#define AC_IF_COMMON_MSG_IN							(3)
#define AC_IF_COMMON_MSG_OUT						(4)
#define AC_IF_DYNAMIC_MSG_IN						(5)
#define AC_IF_DYNAMIC_MSG_OUT						(6)

/*****************************************************************************/
/* UART_IF task define
 */
/*****************************************************************************/
/* timer signal */
/* define signal */
#define AC_UART_IF_PURE_MSG_OUT						(1)
#define AC_UART_IF_COMMON_MSG_OUT					(2)
#define AC_UART_IF_DYNAMIC_MSG_OUT					(3)

/*****************************************************************************/
/*  LIFE task define
 */
/*****************************************************************************/
/* define timer */
#define AC_DISPLAY_LOGO_INTERVAL			(10000)

/* define signal */
#define AC_DISPLAY_INITIAL					(1)
#define AC_DISPLAY_SHOW_ON_LOGO				(2)
#define AC_DISPLAY_SHOW_OFF_LOGO			(3)


/*****************************************************************************/
/* DBG task define
 */
/*****************************************************************************/
/* define timer */
/* define signal */
#define AC_DBG_TEST_1								(1)
#ifdef __cplusplus
}
#endif

#endif //__APP_IF_H__

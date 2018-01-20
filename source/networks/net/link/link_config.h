#ifndef __LINK_CONFIG_H__
#define __LINK_CONFIG_H__

#include "ak.h"

#define LINK_PDU_BUF_SIZE			512
#define LINK_PDU_POOL_SIZE			5

#define LINK_PHY_FRAME_DATA_SIZE	AK_COMMON_MSG_DATA_SIZE

#define LINK_PHY_FRAME_SEND_TO_INTERVAL		50 /* 50 ms */

#define LINK_PHY_FRAME_REV_TO_INTERVAL		25 /* 25 ms */

#endif //__LINK_CONFIG_H__

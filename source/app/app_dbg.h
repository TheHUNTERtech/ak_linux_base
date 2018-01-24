#ifndef __APP_DBG_H__
#define __APP_DBG_H__

#include <stdio.h>
#include "sys_dbg.h"

#define APP_PRINT_EN
#define APP_DBG_EN
#define APP_DBG_SIG_EN

#ifdef APP_PRINT_EN
#define APP_PRINT(fmt, ...)		printf(fmt, ##__VA_ARGS__)
#else
#define APP_PRINT(fmt, ...)
#endif

#ifdef APP_DBG_EN
#define APP_DBG(fmt, ...)		__LOG__(fmt, "APP_DBG", ##__VA_ARGS__)
#else
#define APP_DBG(fmt, ...)
#endif

#ifdef APP_DBG_SIG_EN
#define APP_DBG_SIG(fmt, ...)		__LOG__(fmt, "SIG -> ", ##__VA_ARGS__)
#else
#define APP_DBG_SIG(fmt, ...)
#endif

#endif //__APP_DBG_H__

#include <stdlib.h>
#include "sys_dbg.h"

void sys_dbg_fatal(const char* s, uint8_t c) {
	printf("FATAL: %s \t 0x%02X\n", s, c);
	exit(EXIT_FAILURE);
}

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#include "ak.h"
#include "port.h"
#include "sys_dbg.h"

#include "link_config.h"
#include "link_data.h"

static uint32_t mac_src_add = 0xFFFFFFFF;
static uint32_t mac_des_add = 0xFFFFFFFF;

static link_pdu_t* free_link_pdu_pool;
static link_pdu_t link_pdu_pool[LINK_PDU_POOL_SIZE];

static pthread_mutex_t mt_link_pdu_pool;
static pthread_mutex_t mt_link_addr;

/* link pdu function */
void link_pdu_init() {
	SYS_DBG("[LINK_DATA] link_pdu_init()\n");
	pthread_mutex_lock(&mt_link_pdu_pool);
	free_link_pdu_pool = (link_pdu_t*)link_pdu_pool;
	for (uint32_t i = 0; i < LINK_PDU_POOL_SIZE; i++) {
		link_pdu_pool[i].id = i;
		link_pdu_pool[i].is_used = 0;

		if (i == (LINK_PDU_POOL_SIZE - 1)) {
			link_pdu_pool[i].next = LINK_PDU_NULL;
		}
		else {
			link_pdu_pool[i].next = (link_pdu_t*)&link_pdu_pool[i + 1];
		}
	}
	pthread_mutex_unlock(&mt_link_pdu_pool);
}

link_pdu_t* link_pdu_malloc() {
	link_pdu_t* allocate_msg = free_link_pdu_pool;
	pthread_mutex_lock(&mt_link_pdu_pool);
	if (allocate_msg == LINK_PDU_NULL) {
		FATAL("LINK_PDU", 0x01);
	}
	else {
		allocate_msg->is_used = 1;
		free_link_pdu_pool = free_link_pdu_pool->next;
	}
	pthread_mutex_unlock(&mt_link_pdu_pool);
	SYS_DBG("[LINK_DATA] link_pdu_malloc(%d)\n", allocate_msg->id);
	return allocate_msg;
}

void link_pdu_free(link_pdu_t* link_pdu) {
	SYS_DBG("[LINK_DATA] link_pdu_free(%d)\n", link_pdu->id);
	pthread_mutex_lock(&mt_link_pdu_pool);
	link_pdu->is_used = 0;
	link_pdu->next = free_link_pdu_pool;
	free_link_pdu_pool = link_pdu;
	pthread_mutex_unlock(&mt_link_pdu_pool);
}

link_pdu_t* link_pdu_get(uint32_t pdu_id) {
	SYS_DBG("[LINK_DATA] link_pdu_get(%d)\n", pdu_id);
	return (link_pdu_t*)&link_pdu_pool[pdu_id];
}

void link_pdu_free(uint32_t pdu_id) {
	SYS_DBG("[LINK_DATA] link_pdu_free(%d)\n", pdu_id);
	pthread_mutex_lock(&mt_link_pdu_pool);
	if (pdu_id < LINK_PDU_POOL_SIZE) {
			link_pdu_pool[pdu_id].is_used = 0;
			link_pdu_pool[pdu_id].next = free_link_pdu_pool;
			free_link_pdu_pool = &link_pdu_pool[pdu_id];
	}
	else {
			FATAL("LINK_PDU", 0x02);
	}
	pthread_mutex_unlock(&mt_link_pdu_pool);
}

/* link address utilities */
void link_set_src_addr(uint32_t addr) {
	SYS_DBG("[LINK_DATA] link_set_src_addr(%d)\n", addr);
	pthread_mutex_lock(&mt_link_addr);
	mac_src_add = addr;
	pthread_mutex_unlock(&mt_link_addr);
}

uint32_t link_get_src_addr() {
	uint32_t ret_addr;
	pthread_mutex_lock(&mt_link_addr);
	ret_addr = mac_src_add;
	pthread_mutex_unlock(&mt_link_addr);
	return ret_addr;
}

void link_set_des_addr(uint32_t addr) {
	SYS_DBG("[LINK_DATA] link_set_des_addr(%d)\n", addr);
	pthread_mutex_lock(&mt_link_addr);
	mac_des_add = addr;
	pthread_mutex_unlock(&mt_link_addr);
}

uint32_t link_get_des_addr() {
	uint32_t ret_addr;
	pthread_mutex_lock(&mt_link_addr);
	ret_addr = mac_des_add;
	pthread_mutex_unlock(&mt_link_addr);
	return ret_addr;
}

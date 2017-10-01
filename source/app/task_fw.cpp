#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>

#include "../ak/ak.h"
#include "../ak/timer.h"

#include "../common/firmware.h"

#include "../sys/sys_boot.h"
#include "../sys/sys_dbg.h"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"
#include "app_data.h"

#include "task_list.h"
#include "task_list_if.h"
#include "task_fw.h"
#include "task_if.h"

using namespace std;

typedef struct {
	firmware_header_t fw_header;
	int transfer;
} transfer_fw_status_t;

q_msg_t gw_task_fw_mailbox;

static uint32_t fw_bin_index;
static uint32_t fw_bin_packet_len;

firmware_header_t current_firmware_info;
firmware_header_t file_firmware_info;

string firmware_file_name;

gateway_fw_dev_update_req_t gateway_fw_dev_update_req;

#define GW_FW_PACKED_TIMEOUT_RETRY_COUNTER_MAX		3

static uint8_t gw_fw_packed_timeout_retry_counter;

static void as_sm_release_firmware_update();

static void fw_update_err(uint32_t);
static void fw_no_need_to_update(firmware_header_t*);
static void fw_started_transfer(firmware_header_t*);
static void fw_device_busy();
static void fw_packed_time_out();
static void fw_transfer_status(transfer_fw_status_t*);
static void fw_checksum_err();
static void fw_device_internal_update_started();
static void fw_update_completed();

uint32_t time_out_packet_time = 0;

void* gw_task_fw_entry(void*) {
	string firmware_binary_path = static_cast<string>(APP_ROOT_PATH_DISK) + static_cast<string>("/dev_firmware");

	struct stat st = {0};
	if (stat(firmware_binary_path.data(), &st) == -1) {
		mkdir(firmware_binary_path.data(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}

	task_mask_started();
	wait_all_tasks_started();

	APP_DBG("[STARTED] gw_task_fw_entry\n");

	while (1) {
		while (msg_available(GW_TASK_FW_ID)) {
			/* get messge */
			ak_msg_t* msg = rev_msg(GW_TASK_FW_ID);

			/* handler message */
			switch (msg->header->sig) {
			case GW_FW_UPDATE_COMPLETED: {
				APP_DBG("GW_FW_UPDATE_COMPLETED\n");
				timer_remove_attr(GW_TASK_FW_ID, GW_FW_DEV_INTERNAL_UPDATE_TIMEOUT);
				as_sm_release_firmware_update();
				fw_update_completed();
			}
				break;

			case GW_FW_DEV_INTERNAL_UPDATE_TIMEOUT: {
				APP_DBG("GW_FW_DEV_INTERNAL_UPDATE_TIMEOUT\n");
				as_sm_release_firmware_update();
				fw_update_err(3);
			}
				break;

			case GW_FW_PACKED_TIMEOUT: {
				APP_DBG("GW_FW_PACKED_TIMEOUT\n");

				time_out_packet_time++;

				if (gw_fw_packed_timeout_retry_counter++ > GW_FW_PACKED_TIMEOUT_RETRY_COUNTER_MAX) {
					as_sm_release_firmware_update();
					fw_packed_time_out();
				}
				else {
					ak_msg_t* s_msg = get_pure_msg();
					set_msg_sig(s_msg, GW_FW_TRANFER_REQ);
					set_msg_src_task_id(s_msg, GW_TASK_FW_ID);
					task_post(GW_TASK_FW_ID, s_msg);
				}
			}
				break;

			case GW_FW_OTA_REQ: {
				APP_DBG("GW_FW_OTA_REQ\n");
				time_out_packet_time = 0;
				get_data_dynamic_msg(msg, (uint8_t*)&gateway_fw_dev_update_req, sizeof(gateway_fw_dev_update_req_t));

				firmware_file_name.assign((const char*)gateway_fw_dev_update_req.dev_bin_path);

				APP_DBG("FILE:%s\n", firmware_file_name.data());

				if (stat(firmware_file_name.data(), &st) == -1) {
					APP_DBG("binary file:%s does not exist !\n", firmware_file_name.data());
					fw_update_err(1);
				}
				else {
					/* resend last index of firmware */
					fw_bin_index -= fw_bin_packet_len;

					ak_msg_t* s_msg = get_pure_msg();
					set_msg_sig(s_msg, GW_FW_SM_UPDATE_RES_OK);

					set_msg_src_task_id(s_msg, GW_TASK_FW_ID);
					task_post(GW_TASK_FW_ID, s_msg);
				}
			}
				break;

			case GW_FW_SM_UPDATE_RES_OK: {
				APP_DBG("GW_FW_SM_UPDATE_RES_OK\n");
				ak_msg_t* s_msg = get_pure_msg();
				set_msg_sig(s_msg, GW_IF_PURE_MSG_OUT);

				set_if_src_task_id(s_msg, GW_TASK_FW_ID);
				set_if_des_task_id(s_msg, gateway_fw_dev_update_req.target_task_id);
				set_if_src_type(s_msg, gateway_fw_dev_update_req.source_if_type);
				set_if_des_type(s_msg, gateway_fw_dev_update_req.target_if_type);

				if (gateway_fw_dev_update_req.type_update == TYPE_UPDATE_TARTGET_APP) {
					set_if_sig(s_msg, FW_CRENT_APP_FW_INFO_REQ);
				}
				else if (gateway_fw_dev_update_req.type_update == TYPE_UPDATE_TARTGET_BOOT) {
					set_if_sig(s_msg, FW_CRENT_BOOT_FW_INFO_REQ);
				}
				else {
					FATAL("GFW", 0x01);
				}

				set_msg_src_task_id(s_msg, GW_TASK_FW_ID);
				task_post(GW_TASK_IF_ID, s_msg);

				timer_set(GW_TASK_FW_ID, GW_FW_GET_FIRMWARE_INFO_TIMEOUT, GW_TIMER_FIRMWARE_GET_FIRMWARE_INFO_TIMEOUT, TIMER_ONE_SHOT);
			}
				break;

			case GW_FW_GET_FIRMWARE_INFO_TIMEOUT: {
				APP_DBG("GW_FW_GET_FIRMWARE_INFO_TIMEOUT\n");
				as_sm_release_firmware_update();
				fw_update_err(2);
			}
				break;

			case GW_FW_CURRENT_INFO_RES: {
				timer_remove_attr(GW_TASK_FW_ID, GW_FW_GET_FIRMWARE_INFO_TIMEOUT);

				APP_DBG("GW_FW_CURRENT_INFO_RES\n");
				get_data_common_msg(msg, (uint8_t*)&current_firmware_info, sizeof(firmware_header_t));
				APP_DBG("current firmware checksum: %04X\n", current_firmware_info.checksum);
				APP_DBG("current firmware bin_len: %d\n", current_firmware_info.bin_len);

				firmware_get_info(&file_firmware_info, firmware_file_name.data());
				APP_DBG("file firmware checksum: %04X\n", file_firmware_info.checksum);
				APP_DBG("file firmware bin_len: %d\n", file_firmware_info.bin_len);

				if (current_firmware_info.checksum == file_firmware_info.checksum) {
					APP_DBG("USER_NO_NEED_TO_UPDATE\n");
					as_sm_release_firmware_update();
					fw_no_need_to_update(&current_firmware_info);
				}
				else {
					ak_msg_t* s_msg = get_common_msg();

					set_if_src_task_id(s_msg, GW_TASK_FW_ID);
					set_if_src_type(s_msg, gateway_fw_dev_update_req.source_if_type);
					set_if_des_task_id(s_msg, gateway_fw_dev_update_req.target_task_id);
					set_if_des_type(s_msg, gateway_fw_dev_update_req.target_if_type);
					set_if_sig(s_msg, FW_UPDATE_REQ);
					set_if_data_common_msg(s_msg, (uint8_t*)&file_firmware_info, sizeof(firmware_header_t));

					set_msg_sig(s_msg, GW_IF_COMMON_MSG_OUT);
					set_msg_src_task_id(s_msg, GW_TASK_FW_ID);
					task_post(GW_TASK_IF_ID, s_msg);
				}
			}
				break;

			case GW_FW_UPDATE_RES_OK: {
				APP_DBG("GW_FW_UPDATE_RES_OK\n");
				fw_bin_index = 0;

				ak_msg_t* s_msg = get_pure_msg();
				set_msg_sig(s_msg, GW_FW_TRANFER_REQ);

				set_msg_src_task_id(s_msg, GW_TASK_FW_ID);
				task_post(GW_TASK_FW_ID, s_msg);

				fw_started_transfer(&file_firmware_info);
			}
				break;

			case GW_FW_TRANFER_REQ: {
				uint8_t data_temp[AK_COMMON_MSG_DATA_SIZE];
				uint32_t remain;
				memset(data_temp, 0, AK_COMMON_MSG_DATA_SIZE);

				remain = file_firmware_info.bin_len - fw_bin_index;

				if (remain < AK_COMMON_MSG_DATA_SIZE) {
					fw_bin_packet_len = (uint8_t)remain;
				}
				else {
					fw_bin_packet_len = AK_COMMON_MSG_DATA_SIZE;
				}

				firmware_read(data_temp, fw_bin_index, fw_bin_packet_len, firmware_file_name.data());
				fw_bin_index += fw_bin_packet_len;

				/* reset counter retry */
				gw_fw_packed_timeout_retry_counter = 0;

				if (fw_bin_index < file_firmware_info.bin_len) {
					timer_set(GW_TASK_FW_ID, GW_FW_PACKED_TIMEOUT, GW_TIMER_FIRMWARE_PACKED_TIMEOUT_INTERVAL, TIMER_ONE_SHOT);
				}

				ak_msg_t* s_msg = get_common_msg();

				set_if_src_task_id(s_msg, GW_TASK_FW_ID);
				set_if_src_type(s_msg, gateway_fw_dev_update_req.source_if_type);
				set_if_des_task_id(s_msg, gateway_fw_dev_update_req.target_task_id);
				set_if_des_type(s_msg, gateway_fw_dev_update_req.target_if_type);
				set_if_sig(s_msg, FW_TRANSFER_REQ);
				set_if_data_common_msg(s_msg, data_temp, AK_COMMON_MSG_DATA_SIZE);

				set_msg_sig(s_msg, GW_IF_COMMON_MSG_OUT);
				set_msg_src_task_id(s_msg, GW_TASK_FW_ID);
				task_post(GW_TASK_IF_ID, s_msg);

				float percent = ((float)fw_bin_index / (float)file_firmware_info.bin_len) * (float)100;
				APP_DBG("[transfer] %d bytes %d %c\n", fw_bin_index, (uint32_t)percent, '%');

				transfer_fw_status_t fw_stt;
				fw_stt.fw_header.bin_len = file_firmware_info.bin_len;
				fw_stt.fw_header.checksum = file_firmware_info.checksum;
				fw_stt.fw_header.psk = file_firmware_info.psk;
				fw_stt.transfer = fw_bin_index;
				fw_transfer_status(&fw_stt);
			}
				break;

			case GW_FW_TRANSFER_RES_OK: {
				/* clear packed timeout and trigger next sequence */
				timer_remove_attr(GW_TASK_FW_ID, GW_FW_PACKED_TIMEOUT);
				timer_set(GW_TASK_FW_ID, GW_FW_TRANFER_REQ, GW_TIMER_FIRMWARE_TRANFER_PACKET_PENDING_INTERVAL, TIMER_ONE_SHOT);
			}
				break;

			case GW_FW_INTERNAL_UPDATE_REQ: {
				APP_DBG("GW_FW_INTERNAL_UPDATE_REQ\n");

				timer_remove_attr(GW_TASK_FW_ID, GW_FW_PACKED_TIMEOUT);

				ak_msg_t* s_msg = get_pure_msg();

				set_if_src_task_id(s_msg, GW_TASK_FW_ID);
				set_if_src_type(s_msg, gateway_fw_dev_update_req.source_if_type);
				set_if_des_task_id(s_msg, gateway_fw_dev_update_req.target_task_id);
				set_if_des_type(s_msg, gateway_fw_dev_update_req.target_if_type);

				if (gateway_fw_dev_update_req.type_update == TYPE_UPDATE_TARTGET_APP) {
					set_if_sig(s_msg, FW_INTERNAL_UPDATE_APP_RES_OK);
				}
				else if (gateway_fw_dev_update_req.type_update == TYPE_UPDATE_TARTGET_BOOT) {
					set_if_sig(s_msg, FW_INTERNAL_UPDATE_BOOT_RES_OK);
				}
				else {
					FATAL("GFW", 0x02);
				}

				set_msg_sig(s_msg, GW_IF_PURE_MSG_OUT);
				set_msg_src_task_id(s_msg, GW_TASK_FW_ID);
				task_post(GW_TASK_IF_ID, s_msg);

				timer_set(GW_TASK_FW_ID, GW_FW_DEV_INTERNAL_UPDATE_TIMEOUT, GW_TIMER_FIRMWARE_DEV_INTERNAL_UPDATE_TIMEOUT, TIMER_ONE_SHOT);
				fw_device_internal_update_started();
			}
				break;

			case GW_FW_UPDATE_BUSY: {
				APP_DBG("GW_FW_UPDATE_BUSY\n");
				as_sm_release_firmware_update();
				fw_device_busy();
			}
				break;

			case GW_FW_TRANSFER_CHECKSUM_ERR: {
				APP_DBG("GW_FW_TRANSFER_CHECKSUM_ERR\n");
				timer_remove_attr(GW_TASK_FW_ID, GW_FW_PACKED_TIMEOUT);
				as_sm_release_firmware_update();
				fw_checksum_err();
			}
				break;

			case GW_FW_SM_RELEASE_RES_OK: {
				APP_DBG("GW_FW_SM_RELEASE_RES_OK\n");
			}

			default:
				break;
			}

			free_msg(msg);
		}
	}

	return (void*)0;
}

void as_sm_release_firmware_update() {
	APP_DBG("[FW] as_sm_release_firmware_update\n");

	//	ak_msg_t* s_msg = get_pure_msg();
	//	set_msg_sig(s_msg, GW_AC_SM_FIRMWARE_UPDATE_RELEASE_REQ);

	//	set_msg_src_task_id(s_msg, GW_TASK_FW_ID);
	//	task_post(GW_TASK_AC_SM_ID, s_msg);
}

void fw_update_err(uint32_t err) {
	APP_DBG("[FW] fw_no_need_to_update: %d\n", err);
	//	ak_msg_t* s_msg = get_common_msg();

	//	set_if_src_task_id(s_msg, GW_TASK_FW_ID);
	//	set_if_des_task_id(s_msg, GI_TASK_FIRMWARE_ID);
	//	set_if_des_type(s_msg, IF_TYPE_APP_GI);
	//	set_if_sig(s_msg, GI_FIRMWARE_UPDATE_ERR);
	//	set_if_data_common_msg(s_msg, (uint8_t*)&err, sizeof(uint32_t));

	//	set_msg_sig(s_msg, GW_IF_COMMON_MSG_OUT);
	//	set_msg_src_task_id(s_msg, GW_TASK_FW_ID);
	//	task_post(GW_TASK_IF_ID, s_msg);
}

void fw_no_need_to_update(firmware_header_t* fw) {
	APP_DBG("[FW] fw_no_need_to_update\n");
	//	ak_msg_t* s_msg = get_common_msg();

	//	set_if_src_task_id(s_msg, GW_TASK_FW_ID);
	//	set_if_des_task_id(s_msg, GI_TASK_FIRMWARE_ID);
	//	set_if_des_type(s_msg, IF_TYPE_APP_GI);
	//	set_if_sig(s_msg, GI_FIRMWARE_NO_NEED_TO_UPDATE);
	//	set_if_data_common_msg(s_msg, (uint8_t*)fw, sizeof(firmware_header_t));

	//	set_msg_sig(s_msg, GW_IF_COMMON_MSG_OUT);
	//	set_msg_src_task_id(s_msg, GW_TASK_FW_ID);
	//	task_post(GW_TASK_IF_ID, s_msg);
}

void fw_started_transfer(firmware_header_t* fw) {
	APP_DBG("[FW] fw_started_transfer\n");
	//	ak_msg_t* s_msg = get_common_msg();

	//	set_if_src_task_id(s_msg, GW_TASK_FW_ID);
	//	set_if_des_task_id(s_msg, GI_TASK_FIRMWARE_ID);
	//	set_if_des_type(s_msg, IF_TYPE_APP_GI);
	//	set_if_sig(s_msg, GI_FIRMWARE_STARTED_TRANSFER);
	//	set_if_data_common_msg(s_msg, (uint8_t*)fw, sizeof(firmware_header_t));

	//	set_msg_sig(s_msg, GW_IF_COMMON_MSG_OUT);
	//	set_msg_src_task_id(s_msg, GW_TASK_FW_ID);
	//	task_post(GW_TASK_IF_ID, s_msg);
}

void fw_device_busy() {
	APP_DBG("[FW] fw_device_busy\n");
	//	ak_msg_t* s_msg = get_pure_msg();

	//	set_if_src_task_id(s_msg, GW_TASK_FW_ID);
	//	set_if_des_task_id(s_msg, GI_TASK_FIRMWARE_ID);
	//	set_if_des_type(s_msg, IF_TYPE_APP_GI);
	//	set_if_sig(s_msg, GI_FIRMWARE_DEVICE_BUSY);

	//	set_msg_sig(s_msg, GW_IF_PURE_MSG_OUT);
	//	set_msg_src_task_id(s_msg, GW_TASK_FW_ID);
	//	task_post(GW_TASK_IF_ID, s_msg);
}

void fw_packed_time_out() {
	APP_DBG("[FW] fw_packed_time_out\n");
	//	ak_msg_t* s_msg = get_pure_msg();

	//	set_if_src_task_id(s_msg, GW_TASK_FW_ID);
	//	set_if_des_task_id(s_msg, GI_TASK_FIRMWARE_ID);
	//	set_if_des_type(s_msg, IF_TYPE_APP_GI);
	//	set_if_sig(s_msg, GI_FIRMWARE_PACKED_TIMEOUT);

	//	set_msg_sig(s_msg, GW_IF_PURE_MSG_OUT);
	//	set_msg_src_task_id(s_msg, GW_TASK_FW_ID);
	//	task_post(GW_TASK_IF_ID, s_msg);
}

void fw_transfer_status(transfer_fw_status_t* stt) {
	APP_DBG("[FW] fw_transfer_status\n");
	//	ak_msg_t* s_msg = get_common_msg();

	//	set_if_src_task_id(s_msg, GW_TASK_FW_ID);
	//	set_if_des_task_id(s_msg, GI_TASK_FIRMWARE_ID);
	//	set_if_des_type(s_msg, IF_TYPE_APP_GI);
	//	set_if_sig(s_msg, GI_FIRMWARE_TRANSFER_STATUS);
	//	set_if_data_common_msg(s_msg, (uint8_t*)stt, sizeof(transfer_fw_status_t));

	//	set_msg_sig(s_msg, GW_IF_COMMON_MSG_OUT);
	//	set_msg_src_task_id(s_msg, GW_TASK_FW_ID);
	//	task_post(GW_TASK_IF_ID, s_msg);
}

void fw_checksum_err() {
	APP_DBG("[FW] fw_checksum_err\n");
	//	ak_msg_t* s_msg = get_pure_msg();

	//	set_if_src_task_id(s_msg, GW_TASK_FW_ID);
	//	set_if_des_task_id(s_msg, GI_TASK_FIRMWARE_ID);
	//	set_if_des_type(s_msg, IF_TYPE_APP_GI);
	//	set_if_sig(s_msg, GI_FIRMWARE_CHECKSUM_ERR);

	//	set_msg_sig(s_msg, GW_IF_PURE_MSG_OUT);
	//	set_msg_src_task_id(s_msg, GW_TASK_FW_ID);
	//	task_post(GW_TASK_IF_ID, s_msg);
}

void fw_device_internal_update_started() {
	APP_DBG("[FW] fw_device_internal_update_started\n");
	//	ak_msg_t* s_msg = get_pure_msg();

	//	set_if_src_task_id(s_msg, GW_TASK_FW_ID);
	//	set_if_des_task_id(s_msg, GI_TASK_FIRMWARE_ID);
	//	set_if_des_type(s_msg, IF_TYPE_APP_GI);
	//	set_if_sig(s_msg, GI_FIRMWARE_DEVICE_INTERNAL_UPDATE_STARTED);

	//	set_msg_sig(s_msg, GW_IF_PURE_MSG_OUT);
	//	set_msg_src_task_id(s_msg, GW_TASK_FW_ID);
	//	task_post(GW_TASK_IF_ID, s_msg);
}

void fw_update_completed() {
	APP_DBG("[FW] fw_update_completed\n");
	//	ak_msg_t* s_msg = get_pure_msg();

	//	set_if_src_task_id(s_msg, GW_TASK_FW_ID);
	//	set_if_des_task_id(s_msg, GI_TASK_FIRMWARE_ID);
	//	set_if_des_type(s_msg, IF_TYPE_APP_GI);
	//	set_if_sig(s_msg, GI_FIRMWARE_UPDATE_COMPLETED);

	//	set_msg_sig(s_msg, GW_IF_PURE_MSG_OUT);
	//	set_msg_src_task_id(s_msg, GW_TASK_FW_ID);
	//	task_post(GW_TASK_IF_ID, s_msg);
}

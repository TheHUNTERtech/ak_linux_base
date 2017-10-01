#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "app_data.h"
#include "app_dbg.h"
#include "app_if.h"

uint8_t rf24_encrypt_decrypt_key[] = {0x4C, 0xD4, 0x00, 0x08, 0x37, 0xB5, 0x18, 0x4C, 0x01, 0x21, 0x20, 0x46, 0x04, 0xF0, 0xC7, 0xF9};

if_app_t if_app_list[] = {
	/*	[id]				[socket_path]							[location_path]									[enable_laucher]*/
	{	IF_TYPE_APP_GMNG,	(char*)"/tmp/gateway_app_manager",		(char*)"/usr/local/bin/gateway_app_manager",	ENABLE_LAUNCHER		},
	{	IF_TYPE_APP_GW,		(char*)"/tmp/gateway",					(char*)"/usr/local/bin/gateway",				ENABLE_LAUNCHER		},
	{	IF_TYPE_APP_GI,		(char*)"/tmp/gateway_ui",				(char*)"/usr/local/bin/gateway_ui",				ENABLE_LAUNCHER		},
};

uint32_t if_app_list_size = sizeof(if_app_list)/sizeof(if_app_list[0]);

void show_app_table() {
	APP_DBG("\n[APPS TABLE]\n");
	APP_DBG("%-10s%-40s%-40s%-40s%-40s\n", "[id]", "[socket_path]", "[location_path]", "[enable_laucher]", "[pid]");
	for (uint32_t i = 0; i < if_app_list_size; i++) {
		APP_DBG("%-10d%-30s%-30s%-30s%-30d\n", if_app_list[i].id	\
				, if_app_list[i].socket_path	\
				, if_app_list[i].location_path	\
				, (const char*)((if_app_list[i].enable_launcher == ENABLE_LAUNCHER) ? "ENABLE_LAUNCHER" : "DISABLE_LAUNCHER")	\
				, if_app_list[i].pid);
	}
}

static gw_connection_t gw_connection_data;

/**
 * configure parameter will be updated when app start.
 */
static app_config_parameter_t config_parameter_data;

void app_data_set_gw_connection(gw_connection_t* gw_connect) {
	APP_DBG("[APP_DATA] app_data_set_sl_settings\n");
	if (gw_connect != NULL) {
		memcpy(&gw_connection_data, gw_connect, sizeof(gw_connection_t));
	}
}

void app_data_get_gw_connection(gw_connection_t* gw_connect) {
	APP_DBG("[APP_DATA] app_data_get_sl_settings\n");
	if (gw_connect != NULL) {
		memcpy(gw_connect, &gw_connection_data, sizeof(gw_connection_t));
	}
}

void app_data_set_config_parameter(app_config_parameter_t* config) {
	APP_DBG("[APP_DATA] app_data_set_config_parameter\n");
	if (config != NULL) {
		memcpy(&config_parameter_data, config, sizeof(app_config_parameter_t));
	}
}

void app_data_get_config_parameter(app_config_parameter_t* config) {
	APP_DBG("[APP_DATA] app_data_get_config_parameter\n");
	if (config != NULL) {
		memcpy(&config, &config_parameter_data, sizeof(app_config_parameter_t));
	}
}

uint8_t temperature[SL_TOTAL_SENSOR_TEMPERATURE];	/* *C */
uint8_t humidity[SL_TOTAL_SENSOR_HUMIDITY];			/* RH% */
uint8_t general_input[SL_TOTAL_GENERAL_INPUT];		/* HIGH/LOW */
uint8_t general_output[SL_TOTAL_GENERAL_OUTPUT];	/* HIGH/LOW */
uint8_t fan_pop[SL_TOTAL_FAN_POP];					/* (0 -> 100)% */
uint8_t fan_dev;									/* (0 -> 100)% */
uint8_t power_output_status;						/* ON/OFF */
uint32_t power_output_current;						/* mA */

json sl_sensors_struct_to_json(sl_sensors_t* sl_sensors) {
	json j;
	std::time_t time = std::time(nullptr);

	j["time"] = time;

	if (sl_sensors != NULL) {
		j["sl_sensors"]["temperature_1"]		= sl_sensors->temperature[0];
		j["sl_sensors"]["temperature_2"]		= sl_sensors->temperature[1];
		j["sl_sensors"]["temperature_3"]		= sl_sensors->temperature[2];
		j["sl_sensors"]["temperature_4"]		= sl_sensors->temperature[3];

		j["sl_sensors"]["himidity_1"]			= sl_sensors->humidity[0];
		j["sl_sensors"]["himidity_2"]			= sl_sensors->humidity[1];

		j["sl_sensors"]["general_input_1"]		= sl_sensors->general_input[0];
		j["sl_sensors"]["general_input_2"]		= sl_sensors->general_input[1];
		j["sl_sensors"]["general_input_3"]		= sl_sensors->general_input[2];
		j["sl_sensors"]["general_input_4"]		= sl_sensors->general_input[3];
		j["sl_sensors"]["general_input_5"]		= sl_sensors->general_input[4];
		j["sl_sensors"]["general_input_6"]		= sl_sensors->general_input[5];
		j["sl_sensors"]["general_input_7"]		= sl_sensors->general_input[6];
		j["sl_sensors"]["general_input_8"]		= sl_sensors->general_input[7];
		j["sl_sensors"]["general_input_9"]		= sl_sensors->general_input[8];
		j["sl_sensors"]["general_input_10"]		= sl_sensors->general_input[9];
		j["sl_sensors"]["general_input_11"]		= sl_sensors->general_input[10];
		j["sl_sensors"]["general_input_12"]		= sl_sensors->general_input[11];

		j["sl_sensors"]["general_output_1"]		= sl_sensors->general_output[0];
		j["sl_sensors"]["general_output_2"]		= sl_sensors->general_output[1];
		j["sl_sensors"]["general_output_3"]		= sl_sensors->general_output[2];
		j["sl_sensors"]["general_output_4"]		= sl_sensors->general_output[3];
		j["sl_sensors"]["general_output_5"]		= sl_sensors->general_output[4];
		j["sl_sensors"]["general_output_6"]		= sl_sensors->general_output[5];
		j["sl_sensors"]["general_output_7"]		= sl_sensors->general_output[6];
		j["sl_sensors"]["general_output_8"]		= sl_sensors->general_output[7];
		j["sl_sensors"]["general_output_9"]		= sl_sensors->general_output[8];
		j["sl_sensors"]["general_output_10"]	= sl_sensors->general_output[9];
		j["sl_sensors"]["general_output_11"]	= sl_sensors->general_output[10];
		j["sl_sensors"]["general_output_12"]	= sl_sensors->general_output[11];

		j["sl_sensors"]["fan_pop_1"]			= sl_sensors->fan_pop[0];
		j["sl_sensors"]["fan_pop_2"]			= sl_sensors->fan_pop[1];
		j["sl_sensors"]["fan_pop_3"]			= sl_sensors->fan_pop[2];
		j["sl_sensors"]["fan_pop_4"]			= sl_sensors->fan_pop[3];

		j["sl_sensors"]["fan_dev"]				= sl_sensors->fan_dev;
		j["sl_sensors"]["power_output_status"]	= sl_sensors->power_output_status;
		j["sl_sensors"]["power_output_current"]	= sl_sensors->power_output_current;
	}

	return j;
}

json flood_sensors_struct_to_json(lora_message_t* lora_message) {
	/**
	{
		"flood_sensor":
		{
			"ip": "1.1.1.12",
			"type": 0,
			"data": 0,
		},
		"time": 1495250198
	}
*/
	json j;
	std::time_t time = std::time(nullptr);
	j["time"] = time;

	struct in_addr lora_in_addr;
	lora_in_addr.s_addr = lora_message->header.scr_addr;
	string dev_ip((const char*)inet_ntoa(lora_in_addr));

	if (lora_message != NULL) {
		j["sensor_flood"]["ip"]		= dev_ip;
		j["sensor_flood"]["type"]	= lora_message->header.type;
		j["sensor_flood"]["data"]	= static_cast<uint32_t>(lora_message->data);
	}
	return j;
}

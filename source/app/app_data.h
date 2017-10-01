#ifndef __APP_DATA_H__
#define __APP_DATA_H__
#include <stdint.h>
#include <string>

#include "../sys/sys_boot.h"

#include "../common/json.hpp"

using namespace std;
using json = nlohmann::json;

/******************************************************************************
* interface type
*******************************************************************************/
/** RF24 interface for modules
*/
#define IF_TYPE_RF24_GW						(0)
#define IF_TYPE_RF24_AC						(1)

/******************************************************************************
* Data type of RF24Network
*******************************************************************************/
#define RF24_DATA_COMMON_MSG_TYPE			(1)
#define RF24_DATA_PURE_MSG_TYPE				(2)
#define RF24_DATA_REMOTE_CMD_TYPE			(3)

/** APP interface, communication via socket interface
 *
 */
#define IF_TYPE_APP_START					(100)
#define IF_TYPE_APP_GMNG					(100)
#define IF_TYPE_APP_GW						(101)
#define IF_TYPE_APP_GI						(102)

/** CPU SERIAL interface, communication via uart serial interface
 *
 */
#define IF_TYPE_UART_GW						(120)
#define IF_TYPE_UART_AC						(121)

#define ENABLE_LAUNCHER						(0x01)
#define DISABLE_LAUNCHER					(0x00)

typedef struct {
	uint32_t id;
	char* socket_path;
	char* location_path;
	uint8_t enable_launcher;
	__pid_t pid;
} if_app_t;

extern if_app_t if_app_list[];
extern uint32_t if_app_list_size;

/******************************************************************************
* Common define
*******************************************************************************/
#define APP_ERROR_CODE_TIMEOUT		0x01
#define APP_ERROR_CODE_BUSY			0x02
#define APP_ERROR_CODE_STATE		0x03

/******************************************************************************
* Commom data structure for transceiver data
*******************************************************************************/
#define CONFIGURE_PARAMETER_BUFFER_SIZE		256
#define RF24_ENCRYPT_DECRYPT_KEY_SIZE		16
extern uint8_t rf24_encrypt_decrypt_key[];

#define FIRMWARE_PSK				0x1A2B3C4D
#define FIRMWARE_LOK				0x1234ABCD

#define SENSOR_STATUS_UNKOWN				0x00
#define SENSOR_STATUS_NORMAL				0x01
#define SENSOR_STATUS_WARNING				0x02
#define SENSOR_STATUS_DISCONNECTED			0x03

#define SENSOR_STATUS_CLOSED				0x01
#define SENSOR_STATUS_OPENED				0x02

#define SENSOR_STATUS_HIGH					0x01
#define SENSOR_STATUS_LOW					0x02

#define SL_TOTAL_SENSOR_TEMPERATURE			(4)
#define SL_TOTAL_SENSOR_HUMIDITY			(2)
#define SL_TOTAL_GENERAL_INPUT				(12)
#define SL_TOTAL_GENERAL_OUTPUT				(12)
#define SL_TOTAL_FAN_POP					(4)

typedef struct {
	uint8_t temperature[SL_TOTAL_SENSOR_TEMPERATURE];	/* *C */
	uint8_t humidity[SL_TOTAL_SENSOR_HUMIDITY];			/* RH% */
	uint8_t general_input[SL_TOTAL_GENERAL_INPUT];		/* HIGH/LOW */
	uint8_t general_output[SL_TOTAL_GENERAL_OUTPUT];	/* HIGH/LOW */
	uint8_t fan_pop[SL_TOTAL_FAN_POP];					/* (0 -> 100)% */
	uint8_t fan_dev;									/* (0 -> 100)% */
	uint8_t power_output_status;						/* ON/OFF */
	uint32_t power_output_current;						/* mA */
} sl_sensors_t;

#define GW_CONNECTION_STATUS_UNKNOWN		0x00
#define GW_CONNECTION_STATUS_CONNECTED		0x01
#define GW_CONNECTION_STATUS_DISCONNECTED	0x02

typedef struct {
	uint8_t sl_conn;
} gw_connection_t;

#define SL_POWER_STATUS_ON					0x01
#define SL_POWER_STATUS_OFF					0x02

#define SL_POP_CTRL_MODE_AUTO				0x01
#define SL_POP_CTRL_MODE_MANUAL				0x02

typedef struct {
	uint8_t mode;
	uint8_t power_status;
	uint8_t fan_status[SL_TOTAL_FAN_POP];
} sl_pop_fan_ctrl_t;

typedef struct {
	uint8_t general_output[SL_TOTAL_GENERAL_OUTPUT];
} sl_io_ctrl_t;

typedef struct {
	uint8_t general_output[SL_TOTAL_GENERAL_OUTPUT];
} mqtt_sl_io_control_packet_t;

typedef struct {
	char lora_host[CONFIGURE_PARAMETER_BUFFER_SIZE];
	char mqtt_host[CONFIGURE_PARAMETER_BUFFER_SIZE];
	uint32_t mqtt_port;
	char mqtt_user_name[CONFIGURE_PARAMETER_BUFFER_SIZE];
	char mqtt_psk[CONFIGURE_PARAMETER_BUFFER_SIZE];
} lora_gateway_t;

typedef struct {
	char host[CONFIGURE_PARAMETER_BUFFER_SIZE];
	char gateway_id_prefix[CONFIGURE_PARAMETER_BUFFER_SIZE];
	char gateway_id[CONFIGURE_PARAMETER_BUFFER_SIZE];
	uint32_t port;

	char user_name_view[CONFIGURE_PARAMETER_BUFFER_SIZE];
	char user_psk_view[CONFIGURE_PARAMETER_BUFFER_SIZE];

	char user_name_control[CONFIGURE_PARAMETER_BUFFER_SIZE];
	char user_psk_control[CONFIGURE_PARAMETER_BUFFER_SIZE];
} pop_gateway_t;

typedef struct {
	lora_gateway_t	lora_gateway;
	pop_gateway_t	pop_gateway;
} app_config_parameter_t;

/* device connection status */
#define DEVICE_IS_DISCONNECTED				(0x00)
#define DEVICE_IS_CONNECTED					(0x01)

#define LORA_NODE_REGISTER					(0)
#define LORA_NODE_REPORT					(1)
#define LORA_NODE_KEEP_ALIVE				(2)

#define FLOOD_SENSOR_ACTIVE					(0x01)
#define FLOOD_SENSOR_INACTIVE				(0x02)

typedef struct {
	uint32_t scr_addr;
	uint32_t des_addr;
	uint8_t  type;
} __attribute__((__packed__))lora_header_t;

typedef struct {
	lora_header_t header;
	uint32_t data;
} __attribute__((__packed__))lora_message_t;

/**
 * gw_connection will be updated in handshake state.
 */
extern void app_data_set_gw_connection(gw_connection_t*);
extern void app_data_get_gw_connection(gw_connection_t*);

/**
 * configure parameter will be updated when app start.
 */
extern void app_data_set_config_parameter(app_config_parameter_t*);
extern void app_data_get_config_parameter(app_config_parameter_t*);

extern json sl_sensors_struct_to_json(sl_sensors_t*);
extern json flood_sensors_struct_to_json(lora_message_t*);

#endif //__APP_DATA_H__

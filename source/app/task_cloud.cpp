#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <ctime>
#include <string>
#include <curl/curl.h>

#include "../ak/ak.h"
#include "../ak/timer.h"

#include "../common/json.hpp"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"
#include "app_data.h"

#include "task_list.h"
#include "task_list_if.h"
#include "task_cloud.h"

using namespace std;
using json = nlohmann::json;

q_msg_t gw_task_cloud_mailbox;

class curl_c {
public:
	curl_c() : curl(curl_easy_init()), http_code(0) {}

	~curl_c() {
		if (curl) {
			curl_easy_cleanup(curl);
		}
	}

	string get(const string& url) {
		CURLcode res;
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

		ss.str("");
		http_code = 0;

		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			throw runtime_error(curl_easy_strerror(res));
		}

		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

		return ss.str();
	}

	long get_http_code() {
		return http_code;
	}

private:
	static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
		return static_cast<curl_c*>(userp)->Write(buffer,size,nmemb);
	}

	size_t Write(void *buffer, size_t size, size_t nmemb) {
		ss.write((const char*)buffer,size*nmemb);
		return size*nmemb;
	}

	CURL* curl;
	stringstream ss;
	long http_code;
};

void* gw_task_cloud_entry(void*) {
	task_mask_started();
	wait_all_tasks_started();

	APP_DBG("[STARTED] gw_task_cloud_entry\n");

	while (1) {
		while (msg_available(GW_TASK_CLOUD_ID)) {
			/* get messge */
			ak_msg_t* msg = rev_msg(GW_TASK_CLOUD_ID);

			/* free message */
			free_msg(msg);
		}
	}

	return (void*)0;
}

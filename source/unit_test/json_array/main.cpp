#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <iostream>
#include <vector>
#include <string>
#include <stdbool.h>

#include "json.hpp"

using namespace std;
using json = nlohmann::json;

int main() {
	printf("ut_json_array\n");
//	string json_c_str = "{\"list\": [0.234, 34.34234, 2434.0234]}";
//	string json_c_str = "{\"list\": [\"nguyen\", \"trong\", \"than\"]}";
	string json_c_str = "{\"list\": [41234, 324, 453454]}";
	json json_obj = json::parse(json_c_str.c_str());
	json json_obj_list = json_obj["list"];
	if (json_obj_list.is_array()) {
		printf("array\n");
		for (int i = 0; i < json_obj_list.size(); i++) {
			cout << json_obj_list[i] << endl;
		}
	}
	else {
		printf("NOT array\n");
	}

	string dump = json_obj.dump();
	printf("json_dump: %s\n", dump.c_str());
	return 0;
}

#ifndef SETTINGS_H
#define SETTINGS_H
#include <ArduinoJson.hpp>
#include <stdint.h>

#define SETTING_MODE_AUTO "auto"
#define SETTING_MODE_MANUAL "manual"
#define SETTING_MODE_TEST "test"

#define SETTING_START_MICROSTART "microstart"
#define SETTING_START_5SEC "5sec"

#define SETTING_STRATEGY_BASIC (uint8_t)1

namespace Settings {

	/*typedef struct Setting_t {
		uint8_t mode = SETTING_MODE_AUTO;
		uint8_t strategy = SETTING_STRATEGY_BASIC;
		uint8_t start_mode = SETTING_START_MICROSTART;
		int color_black_limit = 20000;
		uint16_t detect_distance = 50; //in cm
		uint16_t boost_distance = 15;
		uint16_t escape_count_max = 75;
		uint16_t boost_count_max = 200;
	};*/

	ArduinoJson6141_0000010::JsonDocument init();
	void save(ArduinoJson6141_0000010::JsonDocument);
	ArduinoJson6141_0000010::JsonDocument reset();

};

#endif
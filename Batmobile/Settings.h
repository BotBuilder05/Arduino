#ifndef SETTINGS_H
#define SETTINGS_H
#include <ArduinoJson.hpp>
#include <stdint.h>

#define SETTING_MODE_AUTO (uint8_t)1
#define SETTING_MODE_MANUAL (uint8_t)2
#define SETTING_MODE_TEST (uint8_t)3

#define SETTING_START_MICROSTART (uint8_t)1
#define SETTING_START_5SEC (uint8_t)2

#define SETTING_STRATEGY_BASIC (uint8_t)1

namespace Settings {

	constexpr int size_json = JSON_OBJECT_SIZE(8);

	typedef struct Setting_t {
		uint8_t mode = SETTING_MODE_AUTO;
		uint8_t strategy = SETTING_STRATEGY_BASIC;
		uint8_t start_mode = SETTING_START_MICROSTART;
		int color_black_limit = 20000;
		uint16_t detect_distance = 50; //in cm
		uint16_t boost_distance = 15;
		uint16_t escape_count_max = 75;
		uint16_t boost_count_max = 200;
	};

	Setting_t init();
	void save(const Setting_t);
	ArduinoJson6141_0000010::JsonDocument getJson(const Setting_t);
	Setting_t setJson(const ArduinoJson6141_0000010::JsonDocument);
};

#endif
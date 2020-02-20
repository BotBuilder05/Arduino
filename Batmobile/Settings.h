#ifndef SETTINGS_H
#define SETTINGS_H
#include <stdint.h>

#define SETTING_MODE_AUTO (uint8_t)1
#define SETTING_MODE_MANUAL (uint8_t)2

#define SETTING_STRATEGY_BASIC (uint8_t)1

namespace Settings {

	typedef struct Setting_t {
		uint8_t mode = SETTING_MODE_AUTO;
		uint8_t strategy = SETTING_STRATEGY_BASIC;
		int color_black_limit = 20000;
		uint16_t detect_distance = 50; //in cm
		uint16_t boost_distance = 15;
	};

	Setting_t init();
	void save(const Setting_t);
};

#endif
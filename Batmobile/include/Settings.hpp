#ifndef SETTINGS_H
#define SETTINGS_H
#include <ArduinoJson.h>
#include <stdint.h>

#define CONFIG_FILE "/config.json"

#define SETTING_MODE_AUTO "auto"
#define SETTING_MODE_MANUAL "manual"
#define SETTING_MODE_TEST "test"

#define SETTING_START_MICROSTART "microstart"
#define SETTING_START_5SEC "5sec"

#define SETTING_STRATEGY_BASIC "basic"
#define SETTING_STRATEGY_DIFF_START "start"
#define SETTING_STRATEGY_DIFF_START2 "start2"

class Settings {

	public:
		Settings() = delete;

		static DynamicJsonDocument set;

		static void init();
		static void save();
		static boolean setJson(const String &);
};

#endif

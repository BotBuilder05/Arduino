#include <Preferences.h>
#include "Settings.h"

namespace Settings {

	::Preferences EEsetting;

	const int settings_size = JSON_OBJECT_SIZE(8);

	char settings_json[] = "{"
		"\"mode\": \"auto\","
		"\"strategy\": \"basic\","
		"\"start_mode\": \"microstart\","
		"\"color_black_limit\": 20000,"
		"\"detect_distance\": 50,"
		"\"boost_distance\": 15,"
		"\"escape_count_max\": 75,"
		"\"boost_count_max\": 200"
		"}";

	ArduinoJson6141_0000010::JsonDocument init()
	{
		ArduinoJson6141_0000010::StaticJsonDocument<settings_size> set;
		//Setting_t set;
		char buf[ settings_size ];
		EEsetting.begin("settings", false);
		if (EEsetting.getBytesLength("settings") > 0) {
			EEsetting.getBytes("settings", buf, settings_size);
			deserializeJson(set, buf);
		}
		else {
			deserializeJson(set, settings_json);
		}
		return set;
	}

	ArduinoJson6141_0000010::JsonDocument reset() 
	{
		EEsetting.remove("settings");
		return init();
	}

	void save(ArduinoJson6141_0000010::JsonDocument set) {
		char buf[settings_size];
		serializeJson(set, buf);
		EEsetting.putBytes("settings", buf, strlen(buf));
	}

}


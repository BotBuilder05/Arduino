#include <Preferences.h>
#include "Settings.h"

namespace Settings {

	::Preferences EEsetting;

	/*const int settings_size = JSON_OBJECT_SIZE(8);

	char settings[] = "{"
		"\"mode\": \"auto\","
		"\"strategy\": \"basic\","
		"\"start_mode\": \"microstart\","
		"\"color_black_limit\": 20000,"
		"\"detect_distance\": 50,"
		"\"boost_distance\": 15,"
		"\"escape_count_max\": 75,"
		"\"boost_count_max\": 200"
		"}";*/

	Setting_t init()
	{
		Setting_t set;
		char buf[sizeof(Setting_t)];
		EEsetting.begin("settings", false);
		if (EEsetting.getBytesLength("settings") > 0) {
			EEsetting.getBytes("settings", buf, sizeof(Setting_t));
			set = *((Setting_t*)buf);
		}
		return set;
	}

	void save(const Setting_t set) {
		EEsetting.putBytes("settings", &set, sizeof(Setting_t));
	}

	ArduinoJson6141_0000010::JsonDocument getJson(const Setting_t set)
	{
		ArduinoJson6141_0000010::DynamicJsonDocument doc(size_json);
		doc["mode"]				 = set.mode;
		doc["strategy"]			 = set.strategy;
		doc["start_mode"]		 = set.start_mode;
		doc["color_black_limit"] = set.color_black_limit;
		doc["detect_distance"]   = set.detect_distance;
		doc["boost_distance"]	 = set.boost_distance;
		doc["escape_count_max"]	 = set.escape_count_max;
		doc["boost_count_max"]	 = set.boost_count_max;

		return doc;
	}

	Setting_t setJson(const ArduinoJson6141_0000010::JsonDocument doc)
	{
		Setting_t set;
		set.mode			  = doc["mode"] | set.mode;
		set.strategy		  = doc["strategy"] | set.strategy;
		set.start_mode		  = doc["start_mode"] | set.start_mode;
		set.color_black_limit = doc["color_black_limit"] | set.color_black_limit;
		set.detect_distance	  = doc["detect_distance"] | set.detect_distance;
		set.boost_distance	  = doc["boost_distance"] | set.boost_distance;
		set.escape_count_max  = doc["escape_count_max"] | set.escape_count_max;
		set.boost_count_max   = doc["boost_count_max"] | set.boost_count_max;

		save(set);
		return set;
	}

}


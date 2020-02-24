#include <Preferences.h>
#include "Settings.h"

namespace Settings {

	::Preferences EEsetting;

	const int settings_size = JSON_OBJECT_SIZE(8);

	char settings[] = "{"
		"\"mode\": \"auto\","
		"\"strategy\": \"basic\","
		"\"start_mode\": \"microstart\","
		"\"color_black_limit\": 20000,"
		"\"detect_distance\": 50,"
		"\"boost_distance\": 15,"
		"\"escape_count_max\": 75,"
		"\"boost_count_max\": 200"
		"}";

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

}


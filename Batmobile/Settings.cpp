#include <Preferences.h>
#include "Settings.h"

namespace Settings {

	::Preferences EEsetting;

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


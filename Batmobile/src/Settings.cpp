#include <SPIFFS.h>
#include <FS.h>
#include <Settings.hpp>

DynamicJsonDocument Settings::set{500};

void Settings::init()
{
	if(!SPIFFS.begin(true)) {
		Serial.println("[-] Failed to mount filesystem. Formating...");
		vTaskDelay(1000);
		ESP.restart();
		return;
	}
	File f = SPIFFS.open(CONFIG_FILE);
	if(!f) {
		Serial.printf("[-] Failed to open %s. Aborting...\n", CONFIG_FILE);
		return;
	}
	String json;
	while(f.available()) {
		json.concat(f.readString());
	}
	f.close();
	DeserializationError e = deserializeJson(Settings::set, json);
	if(e)
		Serial.printf("[-] Failed to parse json : %s. Aborting...\n", e.c_str());
	return;
}

void Settings::save() {
	File f = SPIFFS.open(CONFIG_FILE, FILE_WRITE);
	if(!f) {
		Serial.printf("[-] Failed to open %s. Aborting...\n", CONFIG_FILE);
		return;
	}
	serializeJson(Settings::set, f);
}

boolean Settings::setJson(const String &json)
{
	DeserializationError e = deserializeJson(Settings::set, json);
	if(e) {
		Serial.printf("[-] Failed to parse json : %s. Aborting...\n", e.c_str());
		return false;
	}
	Settings::save();
	return true;
}

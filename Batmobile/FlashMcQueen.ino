#include <ArduinoJson.hpp>
#include <Preferences.h>
#include <Update.h>
#include <WiFi.h>
#include <Wire.h>
#include <pins_arduino.h>
#include "FlashMcQueen.h"
#include "WifiHandler.h"
#include "Settings.h"

xQueueHandle Global_log_queue, Global_cmd_queue;// , Read_queue;
SemaphoreHandle_t idle_sem;
xTaskHandle WifiTaskH, MainTaskH;
TaskParam_t params;
Settings::Setting_t set;

void setup()
{

	Serial.begin(115200);
	Serial.println("Init...");

	Serial.println("Creating sync objects...");
	Global_log_queue = xQueueCreate(LOG_MAX, LOG_SIZE);
	Global_cmd_queue = xQueueCreate(CMD_MAX, CMD_SIZE);
	//Read_queue = xQueueCreate(1, sizeof(SensorRead_t));
	idle_sem = xSemaphoreCreateBinary();

	set = Settings::init();

	params = { Global_log_queue, Global_cmd_queue, idle_sem, &set };

	Serial.println("Configuring pins");
	pinMode(MICROSTART_EN, OUTPUT);
	pinMode(MICROSTART_IN, INPUT);
	pinMode(BLUE_LED, OUTPUT);

	setupSensors(&set);
	setupMotors();

	Serial.println("Creating tasks...");
	xTaskCreatePinnedToCore(
		IdleLoop,
		"IdleLoop",
		25000,
		NULL,
		1,
		NULL,
		XCORE_2
	);
	
	xTaskCreatePinnedToCore(
		WifiHandling::WifiTask,
		"WifiHandler",
		10000,
		(void*)&params,
		1,
		&WifiTaskH,
		XCORE_1
	);

	Serial.println("Inited !");
}

//main loop code
void IdleLoop(void* pvParams)
{
	Cmd_t cmd[CMD_SIZE];
	String cmds;
	char log[LOG_SIZE];
	for (;;) {
		memset(log, 0, LOG_SIZE);
		if (xQueueReceive(Global_cmd_queue, cmd, 100)) {
			//digitalWrite(BLUE_LED, HIGH);
			Serial.printf("Incoming command %s\n", cmd);
			//parse cmd 
			cmds = String((char *)cmd);
			if (cmds.startsWith(CMD_START)) {
				LOG("Main task started !");
				Serial.println("Creating !");
				xTaskCreatePinnedToCore(
					MainTask,
					"MainTask",
					5000,
					(void*)&params,
					2,
					&MainTaskH,
					XCORE_2
				);
				Serial.println("Idle loop blocked");
				if (xSemaphoreTake(idle_sem, portMAX_DELAY)) {
					vTaskDelete(MainTaskH);
					Serial.println("Main task ended");
				}
			}
			else if (cmds.startsWith(CMD_RESET)) {
				Settings::save(Settings::Setting_t{});
				ESP.restart();
			}
			else if (cmds.startsWith(CMD_GET)) {
				//cmds = cmds.substring(strlen(CMD_GET) + 1);
				ArduinoJson6141_0000010::JsonDocument json = Settings::getJson(set);
				serializeJson(json, log);
				LOG(log);
			}
			else if (cmds.startsWith(CMD_SET)) {
				cmds = cmds.substring(strlen(CMD_SET)+1);
				if (cmds.startsWith(CMD_SET_MODE)) {
					cmds = cmds.substring(strlen(CMD_SET_MODE)+1);
					if (cmds.startsWith(CMD_MODE_AUTO)) {
						set.mode = SETTING_MODE_AUTO;
						strcat(log, "Setted mode to auto\n");
					}
					else if (cmds.startsWith(CMD_MODE_MANUAL)) {
						set.mode = SETTING_MODE_MANUAL;
						strcat(log, "Setted mode to manual\n");
					}
					else if (cmds.startsWith(CMD_MODE_TEST)) {
						set.mode = SETTING_MODE_TEST;
						strcat(log, "Setted mode to test\n");
					}
					Settings::save(set);
				}
				else if (cmds.startsWith(CMD_SETGET_JSON)) {
					cmds = cmds.substring(strlen(CMD_SETGET_JSON) + 1);
					ArduinoJson6141_0000010::StaticJsonDocument<Settings::size_json> doc;
					deserializeJson(doc, cmds.c_str());
					set = Settings::setJson(doc);
					strcat(log, "Json added\n");
				}
				strcat(log, "Configuration saved !");
				LOG(log);
			}
		} 
		vTaskDelay(500);
	}
}

void loop(void) {  }
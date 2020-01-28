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
		5000,
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
	for (;;) {
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
			else if (cmds.startsWith(CMD_SET)) {
				cmds = cmds.substring(4);
				if (cmds.startsWith(CMD_SET_MODE)) {
					cmds = cmds.substring(5);
					if (cmds.startsWith("1")) {
						set.mode = SETTING_MODE_AUTO;
						LOG("Setted mode to auto");
					}
					else if (cmds.startsWith("2")) {
						set.mode = SETTING_MODE_MANUAL;
						LOG("Setted mode to manual");
					}
				}
				Settings::save(set);
				LOG("Configuration saved !");
			}
		} 
		vTaskDelay(500);
	}
}

void loop(void) {  }
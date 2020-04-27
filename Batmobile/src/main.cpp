#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <Wire.h>
#include <pins_arduino.h>
#include "Batmobile.h"
#include <WifiHandler.hpp>
#include <Settings.hpp>

xQueueHandle Global_log_queue, Global_cmd_queue;// , Read_queue;
SemaphoreHandle_t idle_sem;
xTaskHandle MainTaskH;
TaskParam_t params;

void setup()
{

	Serial.begin(115200);
	Serial.println("Init...");

	Serial.println("Creating sync objects...");
	Global_log_queue = xQueueCreate(LOG_MAX, LOG_SIZE);
	Global_cmd_queue = xQueueCreate(CMD_MAX, CMD_SIZE);
	idle_sem = xSemaphoreCreateBinary();

	Serial.println("Init settings ...");
	Settings::init();

	params = { Global_log_queue, Global_cmd_queue, idle_sem };

	Serial.println("Configuring general pins");
	pinMode(MICROSTART_EN, OUTPUT);
	pinMode(MICROSTART_IN, INPUT);
	pinMode(BLUE_LED, OUTPUT);

	setupSensors();
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

	WifiHandler::begin((void *)&params);

	Serial.println("Inited !");
}

//main loop code
void IdleLoop(void* pvParams)
{
	Cmd_t cmd[CMD_SIZE];
	String cmds;
	for (;;) {
		if (xQueueReceive(Global_cmd_queue, cmd, 100)) {
			Serial.printf("Incoming command %s\n", cmd);
			//parse cmd
			cmds = String{(char *)cmd};
			if (cmds.startsWith(CMD_START)) {
				Serial.println("[+] Creating main task");
				xTaskCreatePinnedToCore(
					MainTask,
					"MainTask",
					5000,
					(void*)&params,
					2,
					&MainTaskH,
					XCORE_2
				);
				WifiHandler::_client.println("[+] Main task started");
				Serial.println("Idle loop blocked");
				if (xSemaphoreTake(idle_sem, portMAX_DELAY)) {
					vTaskDelete(MainTaskH);
					WifiHandler::_client.println("[-] Main task ended");
					Serial.println("[-] Main task ended");
				}
			}
			else if (cmds.startsWith(CMD_RESET)) {
				Settings::save();
				ESP.restart();
			}
			else if (cmds.startsWith(CMD_GET)) {
				//cmds = cmds.substring(strlen(CMD_GET) + 1);
				serializeJson(Settings::set, WifiHandler::_client);
			}
			else if (cmds.startsWith(CMD_SET)) {
				cmds = cmds.substring(strlen(CMD_SET)+1);
				if (cmds.startsWith(CMD_SET_MODE)) {
					cmds = cmds.substring(strlen(CMD_SET_MODE)+1);
					if (cmds.startsWith(CMD_MODE_AUTO)) {
						Settings::set["mode"] = SETTING_MODE_AUTO;
						WifiHandler::_client.println("Setted mode to auto\n");
					}
					else if (cmds.startsWith(CMD_MODE_MANUAL)) {
						Settings::set["mode"] = SETTING_MODE_MANUAL;
						WifiHandler::_client.println("Setted mode to manual\n");
					}
					else if (cmds.startsWith(CMD_MODE_TEST)) {
						Settings::set["mode"] = SETTING_MODE_TEST;
						WifiHandler::_client.println("Setted mode to test");
					}
					Settings::save();
				}
				else if (cmds.startsWith(CMD_SETGET_JSON)) {
					cmds = cmds.substring(strlen(CMD_SETGET_JSON) + 1);
					if(Settings::setJson(cmds))
						WifiHandler::_client.println("[+] Json saved");
					else
					    WifiHandler::_client.println("[-] Failed to save json");
				}
				WifiHandler::_client.println("[+] Configuration saved");
			}
		}
		vTaskDelay(500);
	}
}

void loop(void) {  }

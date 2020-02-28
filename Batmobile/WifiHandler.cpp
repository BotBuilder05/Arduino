#include <WifiServer.h>
#include <WiFiClient.h>
#include <WiFi.h>
#include <Update.h>
#include "WifiHandler.h"

namespace WifiHandling {

	WiFiClient client;
	WiFiServer server;

	void WifiTask(void* pvParams)
	{
		Serial.println("Wifitask, getting params..");
		Cmd_t buf[LOG_SIZE];
		String cmd;
		xQueueHandle log_queue = ((TaskParam_t*)pvParams)->log;
		xQueueHandle cmd_queue = ((TaskParam_t*)pvParams)->cmd;

		Serial.println("Creating wifi...");
		WiFi.softAP(AP_SSID, AP_PASS);
		//WiFi.softAPConfig(IPAddress(LOCAL_IP), IPAddress(GATEWAY_IP), IPAddress(SUBNET_IP));
		server.begin(SERVER_PORT);
		Serial.println(WiFi.softAPIP());

		for (;;) {
			client = server.available();
			
			if (client) {
				Serial.println("Client connected !");
				client.printf("\n=== Batmobile ===\n<<<< Hello %s >>>>\n", client.remoteIP().toString().c_str());
				//if there is log to write
				while (client.connected()) {
					
					if (xQueueReceive(log_queue, buf, 10)) {
						//write log
						client.printf("%s\n", (char*)buf);
					}
					vTaskDelay(10);
					if (client.available()) {
						digitalWrite(BLUE_LED, HIGH);
						cmd = client.readStringUntil('\n');
						//if there is an update action ota
						if (cmd.startsWith(CMD_STOP)) {
							Serial.println("Stopping main task !");
							stopMainTask();
							client.println("Main task stopped");
						}
						else if (cmd.startsWith(CMD_UPDATE)) {

							Serial.println("Begin ota");
							digitalWrite(BLUE_LED, LOW);

							int update_size, written = 0;
							uint8_t buffer[4096];
							update_size = cmd.substring(4).toInt();

							//do the update
							if (Update.begin(update_size, U_FLASH, BLUE_LED)) {
								client.printf("READY %d\n", update_size);
								vTaskDelay(50);
								size_t readLen = 0;
								do {
									digitalWrite(BLUE_LED, HIGH);
									readLen = client.readBytes(buffer, 4096);
									digitalWrite(BLUE_LED, LOW);
									written += Update.write(buffer, readLen);
									vTaskDelay(20);
									client.print("OK");
								} while (Update.remaining() > 0);
								
								Serial.printf("%d written/%d\n", written, update_size);
								if (written == update_size && Update.end()) {
									client.printf("Succefuly written %d bytes.\nRebooting...\n", written);
									digitalWrite(BLUE_LED, LOW);
									vTaskDelay(100);
									ESP.restart();
								}
								else {
									client.print("Error while updating : ");
									Update.printError(client);
									vTaskDelay(100);
								}
							}
							else
								client.printf("Not enough space !\n");
						}
						else {
							xQueueSend(cmd_queue, cmd.c_str(), 10);
							if (xQueueReceive(log_queue, buf, 100)) {
								//write log
								if(strlen((char *)buf) > 0)
									client.printf("%s\n", (char*)buf);
							}
						}
						digitalWrite(BLUE_LED, LOW);
					}
				}
			}
			vTaskDelay(500);
		}
	}

}

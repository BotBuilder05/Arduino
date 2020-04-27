#include <Update.h>
#include <WifiHandler.hpp>

xTaskHandle WifiHandler::_th;
WiFiClient WifiHandler::_client;
WiFiServer WifiHandler::_server;

void WifiHandler::begin(void *params)
{
	xTaskCreatePinnedToCore(
		WifiHandler::Task,
		"WifiHandler",
		10000,
		params,
		1,
		&WifiHandler::_th,
		XCORE_1
	);
}

void WifiHandler::OtaUpdate(int update_size)
{
	Serial.println("Begin ota");
	digitalWrite(BLUE_LED, LOW);

	int written = 0;
	uint8_t buffer[4096];

	//do the update
	if (Update.begin(update_size, U_FLASH, BLUE_LED)) {
		_client.printf("READY %d\n", update_size);
		vTaskDelay(50);
		size_t readLen = 0;
		do {
			digitalWrite(BLUE_LED, HIGH);
			readLen = _client.readBytes(buffer, 4096);
			digitalWrite(BLUE_LED, LOW);
			written += Update.write(buffer, readLen);
			vTaskDelay(20);
			_client.print("OK");
		} while (Update.remaining() > 0);

		Serial.printf("%d written/%d\n", written, update_size);
		if (written == update_size && Update.end()) {
			_client.printf("Succefuly written %d bytes.\nRebooting...\n", written);
			digitalWrite(BLUE_LED, LOW);
			vTaskDelay(100);
			ESP.restart();
		}
		else {
			_client.print("Error while updating : ");
			Update.printError(_client);
			vTaskDelay(100);
		}
	}
	else
		_client.printf("Not enough space !\n");
}

void WifiHandler::Task(void* pvParams)
{
	Serial.println("Wifitask, getting params..");
	Cmd_t buf[LOG_SIZE];
	String cmd;
	xQueueHandle log_queue = ((TaskParam_t*)pvParams)->log;
	xQueueHandle cmd_queue = ((TaskParam_t*)pvParams)->cmd;

	Serial.println("Creating wifi...");
	WiFi.softAP(AP_SSID, AP_PASS);
	//WiFi.softAPConfig(IPAddress(LOCAL_IP), IPAddress(GATEWAY_IP), IPAddress(SUBNET_IP));
	_server.begin(SERVER_PORT);
	Serial.println(WiFi.softAPIP());

	for (;;) {
		_client = _server.available();

		if (_client) {
			Serial.println("Client connected !");
			_client.printf("\n=== Batmobile ===\n<<<< Hello %s >>>>\n", _client.remoteIP().toString().c_str());
			//if there is log to write
			while (_client.connected()) {

				if (_client.available()) {
					digitalWrite(BLUE_LED, HIGH);
					cmd = _client.readStringUntil('\n');
					//if there is an update action ota
					if (cmd.startsWith(CMD_STOP)) {
						Serial.println("Stopping main task !");
						stopMainTask();
						_client.println("Main task stopped");
					}
					else if (cmd.startsWith(CMD_UPDATE)) {

						WifiHandler::OtaUpdate(cmd.substring(4).toInt());

					}
					else {
						xQueueSend(cmd_queue, cmd.c_str(), 10);
						if (xQueueReceive(log_queue, buf, 100)) {
							//write log
							if(strlen((char *)buf) > 0)
								_client.printf("%s\n", (char*)buf);
						}
					}
					digitalWrite(BLUE_LED, LOW);
				}
				vTaskDelay(50);
			}
			stopMainTask();
		}
		vTaskDelay(500);
	}
}

#include <Update.h>
#include <ArduinoOTA.h>
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
	Serial.println("[+] Begin ota update");

	ArduinoOTA.setRebootOnSuccess(true);
	ArduinoOTA.setMdnsEnabled(false);
	ArduinoOTA.begin();
	_client.println("[+] Ready for an update");
	/*digitalWrite(BLUE_LED, LOW);

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
			_client.printf("[+] Succefuly written %d bytes.\nRebooting...\n", written);
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
		_client.printf("[-] Not enough space !\n");*/
}

void WifiHandler::Task(void* pvParams)
{
	Serial.print("[+] Creating wifi AP ");
	String cmd;
	xQueueHandle cmd_queue = ((TaskParam_t*)pvParams)->cmd;

	WiFi.softAP(AP_SSID, AP_PASS);
	_server.begin(SERVER_PORT);
	Serial.printf("[OK]\nLocal ip is : %s\n", WiFi.softAPIP().toString().c_str());

	for (;;) {
		_client = _server.available();

		if (_client) {
			Serial.println("[+] [Wifi] Client connected");
			_client.printf(
			"'########:::::'###::::'########:'##::::'##::'#######::'########::'####:'##:::::::'########:\n"
			" ##.... ##:::'## ##:::... ##..:: ###::'###:'##.... ##: ##.... ##:. ##:: ##::::::: ##.....::\n"
			" ##:::: ##::'##:. ##::::: ##:::: ####'####: ##:::: ##: ##:::: ##:: ##:: ##::::::: ##:::::::\n"
			" ########::'##:::. ##:::: ##:::: ## ### ##: ##:::: ##: ########::: ##:: ##::::::: ######:::\n"
			" ##.... ##: #########:::: ##:::: ##. #: ##: ##:::: ##: ##.... ##:: ##:: ##::::::: ##...::::\n"
			" ##:::: ##: ##.... ##:::: ##:::: ##:.:: ##: ##:::: ##: ##:::: ##:: ##:: ##::::::: ##:::::::\n"
			" ########:: ##:::: ##:::: ##:::: ##:::: ##:. #######:: ########::'####: ########: ########:\n"
			"........:::..:::::..:::::..:::::..:::::..:::.......:::........:::....::........::........::\n"
			);
			_client.printf("> Hello %s\n", _client.remoteIP().toString().c_str());
			//if there is log to write
			while (_client.connected()) {

				if (_client.available()) {
					digitalWrite(BLUE_LED, HIGH);
					cmd = _client.readStringUntil('\n');
					//if there is an update action ota
					if (cmd.startsWith(CMD_STOP)) {
						Serial.println("[+] Stopping main task");
						stopMainTask();
						vTaskDelay(50);
						stopMainTask();
						_client.println("[+] Main task stopped");
					}
					else if (cmd.startsWith(CMD_UPDATE)) {

						WifiHandler::OtaUpdate(cmd.substring(4).toInt());

					}
					else {
						xQueueSend(cmd_queue, cmd.c_str(), 10);
					}
					digitalWrite(BLUE_LED, LOW);
				}
				vTaskDelay(50);
			}
		}
		vTaskDelay(500);
	}
}

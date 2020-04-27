#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H

#include <WiFiServer.h>
#include <WiFiClient.h>
#include <WiFi.h>
#include "Batmobile.h"

class WifiHandler {

	public:
		WifiHandler() = delete;
		static void begin(void *);
		static void Task(void* pvParams);

		static xTaskHandle _th;
		static WiFiClient _client;
		static WiFiServer _server;

	private:
		static void OtaUpdate(int);

};

#endif

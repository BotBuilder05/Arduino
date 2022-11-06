/*********
  Modified from the examples of the Arduino LoRa library
  More resources: https://randomnerdtutorials.com
*********/

//Add bluetooth for logs
//#include "BluetoothSerial.h"
#include <SPI.h>
#include <LoRa.h>
#include "WiFi.h" // Enables the ESP32 to connect to the local network (via WiFi)
#include "FS.h"
#include "WebServer.h" // Enable WebServer

//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

#define GATE 22

// WiFi
const char* ssid = "velours";                 // Your personal network SSID
const char* password = "valentineaudreyxavier"; // Your personal network password
//const char* ssid = "TuxAlp2";                 // Your personal network SSID
//const char* password = "zomu6637"; // Your personal network password
// End WiFi

// Initialise Blustooth
//BluetoothSerial SerialBT;

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;

// Webserver
WebServer server(80);
int order=0;


void handleRoot() {
  String message="<h1>Control your ESP32 by your Browser</h1>";
  message += "Minimal version, just one GATE</BR></BR>";
  server.send(200, "text/html", message);
  order=0; // set Signal to 0 in order to be able to reopen the gate
}

void sendGateOrder(){
      digitalWrite(GATE,HIGH);
  LoRa.beginPacket();
  Serial.print ("Order value: ");
  Serial.println(order);
  LoRa.print(order);
  // DEBUG LoRa.print("Hello");
  LoRa.endPacket();
  Serial.println("Order sent via LoRa!");
      delay(500);
      digitalWrite(GATE,LOW);
}

void gateon(){
  order=1; // set Signal for gate
  server.send(200, "text/html", "Opening gate...");
  Serial.print ("Order value gateon: ");
  Serial.println(order);
  sendGateOrder();
}

void setup() {
  // Set pin mode
  pinMode(GATE,OUTPUT);

  //initialize Serial Monitor
  Serial.begin(115200);
  //SerialBT.begin("ESP32BT2");

  while (!Serial);
  // LoRa
  Serial.println("LoRa Sender");
  //SerialBT.println("LoRa Receiver");

  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);

  //replace the LoRa.begin(---E-) argument with your location's frequency
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America
  while (!LoRa.begin(866E6)) {
    Serial.print(".");
    delay(500);
  }
  // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");


  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialise the WiFi and MQTT Client objects
  WiFiClient wifiClient;


  // Webserver
  server.on("/",handleRoot);
  server.on("/gate_open", gateon);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

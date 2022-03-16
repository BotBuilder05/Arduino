/*********
  Modified from the examples of the Arduino LoRa library
  More resources: https://randomnerdtutorials.com
*********/

//Add bluetooth for logs
//#include "BluetoothSerial.h"
#include <SPI.h>
#include <LoRa.h>
#include "WiFi.h" // Enables the ESP32 to connect to the local network (via WiFi)
#include "PubSubClient.h" // Connect and publish to the MQTT broker
#include "FS.h"
#include "WebServer.h" // Enable WebServer

//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

// WiFi
const char* ssid = "velours";                 // Your personal network SSID
const char* password = "valentineaudreyxavier"; // Your personal network password
//const char* ssid = "TuxAlp2";                 // Your personal network SSID
//const char* password = "zomu6637"; // Your personal network password
// End WiFi

// MQTT
//const char* mqtt_server = "ks2g.tuxalp.com";  // IP of the MQTT broker
const char* mqtt_server = "ks2g.tuxalp.com";  // IP of the MQTT broker
const char* mqtt_username = "solarmon"; // MQTT username
const char* mqtt_password = "MonSolar@2021"; // MQTT password
const char* clientID = "solarESP1"; // MQTT client ID
const char* humidity_topic = "test";
// End MQTT

// Initialise Blustooth
//BluetoothSerial SerialBT;

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;

// Webserver
WebServer server(80);
int order=0;

// 1883 is the listener port for the Broker
PubSubClient client(mqtt_server, 1883, wifiClient);

// Custom function to connet to the MQTT broker via WiFi
void connect_MQTT(){

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.print("Connection to MQTT Broker failed... Status code: ");
    Serial.println(client.state());
  }
}

void handleRoot() {
  String message="<h1>Control your ESP32 by your Browser</h1>";
  message += "Minimal version, just one GATE</BR></BR>";
  server.send(200, "text/html", message);
  order=0; // set Signal to 0 in order to be able to reopen the gate
}

void sendGateOrder(){
  LoRa.beginPacket();
  Serial.print ("Order value: ");
  Serial.println(order);
  LoRa.print(order);
  LoRa.endPacket();
  Serial.println("Order sent via LoRa!");
}

void gateon(){
  order=1; // set Signal for gate
  server.send(200, "text/html", "Opening gate...");
  sendGateOrder();
}


void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  //SerialBT.begin("ESP32BT2");

  while (!Serial);
  // LoRa
  Serial.println("LoRa Receiver");
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

  // 1883 is the listener port for the Broker
  PubSubClient client(mqtt_server, 1883, wifiClient);

  // Webserver
  server.on("/",handleRoot);
  server.on("/gate_open", gateon);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  String LoRaData = "";
  Serial.print(".");
  //Serial.println(packetSize);
  if (packetSize) {
    // received a packet
    Serial.print("Received packet ");
    connect_MQTT();
    if (!client.connected()) {
      Serial.print(" : ");
      Serial.println("mqtt_reconnect");
      connect_MQTT();
    }
    client.loop();

    // read packet
    while (LoRa.available()) {
      //String LoRaData = LoRa.readString();
      LoRaData = LoRa.readString();
      //LoRaData = "DATA";
      Serial.print("Data: "); 
      Serial.println(LoRaData); 
    }
    // print RSSI of packet
    Serial.print(" with RSSI ");
    Serial.println(LoRa.packetRssi());
    //https://diyi0t.com/microcontroller-to-raspberry-pi-wifi-mqtt-communication/
    // PUBLISH to the MQTT Broker (topic = Humidity, defined at the beginning)
    //if (client.publish(humidity_topic, "TEST1")) {
    if (client.publish(humidity_topic, LoRaData.c_str())) {
      Serial.println("Message sent to mosquitto over Wifi!");
    }
    // Again, client.publish will return a boolean value depending on whether it succeded or not.
    // If the message failed to send, we will try again, as the connection may have broken.
    else {
      Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
      client.connect(clientID, mqtt_username, mqtt_password);
      delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
      //client.publish(humidity_topic, "TEST2");
      client.publish(humidity_topic, LoRaData.c_str());
      //client.publish(humidity_topic, LoRaData.c_str());
    }
    client.disconnect();  // disconnect from the MQTT broker
    //delay(1000*60);       // print new values every 1 Minute
  }
  else {
      LoRaData = "NO_DATA";
  }
  server.handleClient();
}

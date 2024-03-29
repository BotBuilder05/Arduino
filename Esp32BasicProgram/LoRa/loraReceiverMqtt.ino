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

//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

// WiFi
//const char* ssid = "velours";                 // Your personal network SSID
//const char* password = "valentineaudreyxavier"; // Your personal network password
const char* ssid = "TuxAlp2";                 // Your personal network SSID
const char* password = "zomu6637"; // Your personal network password
// End WiFi


// MQTT
const char* mqtt_server = "ks2g.tuxalp.com";  // IP of the MQTT broker
const char* mqtt_username = "solarmon"; // MQTT username
const char* mqtt_password = "MonSolar@2021"; // MQTT password
const char* clientID = "solarESP1"; // MQTT client ID

// See if we can get the CPTID/ADCO from LoRa data to set topic dynamically
// TEST
const char* myvar="{\"_UPTIME\":28739,\"ADCO\":2147483647,\"OPTARIF\":\"BASE\",\"ISOUSC\":30,\"BASE\":37619556,\"PTEC\":\"TH..\",\"IINST\":2,\"IMAX\":90,\"PAPP\":510,\"HHPHC\":\"A\",\"MOTDETAT\":0}";
#include <iostream>
using namespace std;
#include<string>
// END TEST

const char* topic = "test";
// End MQTT

// Initialise Blustooth
//BluetoothSerial SerialBT;

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;

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

string printString(string str, char ch, int count)
{
    int occ = 0, i;

    // If given count is 0
    // print the given string and return
    if (count == 0) {
        cout << str;
        return str;
    }

    // Start traversing the string
    for (i = 0; i < str.length(); i++) {

        // Increment occ if current char is equal
        // to given character
        if (str[i] == ch)
            occ++;

        // Break the loop if given character has
        // been occurred given no. of times
        if (occ == count)
            break;
    }

    // Print the string after the occurrence
    // of given character given no. of times
    if (i < str.length() - 1) {
        //cout << i << endl;
        cout << str.substr(i +3,10) << endl;
        return str.substr(i +3,10);
        //cout << str.substr(20,10) << endl;
        //cout << str.substr(i + 2, str.length() - (i + 1));
    }
    // Otherwise string is empty
    else
        cout << "Empty string";
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

  // Initialise the WiFi and MQTT Client objects
  //WiFiClient wifiClient;
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

// TEST


  Serial.print("MyVar: ");
  Serial.println(myvar);
  Serial.print("My ACDO: ");
  string mystring="";
  mystring=printString(myvar, 'O', 1);

  topic=mystring.c_str();

  Serial.print("Topic: ");
  Serial.println(topic);

        if (!client.connected()) {
          Serial.println("mqtt_reconnect");
          connect_MQTT();
        }
        client.loop();
        if (client.publish(topic, myvar,true)) {
            Serial.println("Message sent to mosquitto over Wifi!");
        }
// ENDTEST
  // 1883 is the listener port for the Broker
  //PubSubClient client(mqtt_server, 1883, wifiClient);
  //connect_MQTT();
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  String LoRaData = "";
//  Serial.print("PacketSize: ");
//  Serial.println(packetSize);
  if (packetSize) {
    // received a packet
    Serial.print("Received packet ");

    // read packet
    while (LoRa.available()) {
      LoRaData = LoRa.readString();
      Serial.print("Data: "); 
      Serial.println(LoRaData); 
      Serial.println(LoRaData.indexOf('UPTIME'));
    }
    // print RSSI of packet
    //Serial.print(" with RSSI ");
    //Serial.println(LoRa.packetRssi());
        //https://diyi0t.com/microcontroller-to-raspberry-pi-wifi-mqtt-communication/
        // PUBLISH to the MQTT Broker (topic = Humidity, defined at the beginning)
        //if (client.publish(topic, "TEST1")) {
      if (LoRaData.indexOf("_UPTIME") != -1){
        Serial.print("FOUND UPTIME: "); 
        Serial.println(LoRaData); 
        if (!client.connected()) {
          Serial.println("mqtt_reconnect");
          connect_MQTT();
        }
        client.loop();

        if (client.publish(topic, LoRaData.c_str(),true)) {
            Serial.println("Message sent to mosquitto over Wifi!");
        }
        // Again, client.publish will return a boolean value depending on whether it succeded or not.
        // If the message failed to send, we will try again, as the connection may have broken.
        else {
            Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
            //client.connect(clientID, mqtt_username, mqtt_password);
            connect_MQTT();
            delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
            // We should set an infinite loop to be sure message will be sent
            if (client.publish(topic, LoRaData.c_str(),true)) {
                Serial.println("Message send to mosquitto over Wifi at second try!!");
            }
            else {
                Serial.println("Message lost");
            }
        }
        client.disconnect();  // disconnect from the MQTT broker
        Serial.println("DISCONNECT from MQTT "); 
      }
      else {
        Serial.println("UPTIME NOT FOUND!!! No MQTT connection needed"); 
      }
  }
  else {
      LoRaData = "NO_DATA";
  }
}

/*********
  Modified from the examples of the Arduino LoRa library
  More resources: https://randomnerdtutorials.com
  DHT : https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-sensor-arduino-ide/
*********/

//Add bluetooth for logs
//#include "BluetoothSerial.h"
#include <SPI.h>
#include <LoRa.h>

//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

#define GATE 22

//BluetoothSerial SerialBT;
void setup() {
  // Set pin mode
  pinMode(GATE,OUTPUT);

  //initialize Serial Monitor
  Serial.begin(115200);
//  SerialBT.begin("ESP32BT");
  while (!Serial);
  Serial.println("LoRa Receiver");
//  SerialBT.println("LoRa Sender");

  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);

  //replace the LoRa.begin(---E-) argument with your location's frequency
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America
  while (!LoRa.begin(866E6)) {
    Serial.print(".");
//    SerialBT.print(".");
    delay(500);
  }
   // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
//  SerialBT.println("LoRa Initializing OK!");
}

void driveGate(){
  digitalWrite(GATE,HIGH);
  delay(1000);
  digitalWrite(GATE,LOW);
}

void manageGateSignal() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  String LoRaData = "";

  // DEBUG Serial.print("PacketSize:");
  // DEBUG Serial.println(packetSize);

  if (packetSize) {
    // received a packet
    Serial.print("Received packet ");
    }

  // DEBUG Serial.println("On est la ");
  // read packet
  while (LoRa.available()) {
    //String LoRaData = LoRa.readString();
    // DEBUG Serial.println("On est encore la ");
    LoRaData = LoRa.readString();
    //LoRaData = "DATA";
    Serial.print("Data: "); 
    Serial.println(LoRaData); 
    // print RSSI of packet
    Serial.print(" with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
  if ((packetSize) && (LoRaData)) {
    driveGate();
    // DEBUG Serial.println("On est passe par driveGate ");
  }
  else {
    LoRaData = "NO_DATA";
  }
}


void loop() {
  manageGateSignal();
}


/*********
  Modified from the examples of the Arduino LoRa library
  More resources: https://randomnerdtutorials.com
*********/

//Add bluetooth for logs
#include "BluetoothSerial.h"
#include <SPI.h>
#include <LoRa.h>

//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

BluetoothSerial SerialBT;

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  SerialBT.begin("ESP32BT2");

  Serial.println("The device started, now you can pair it with bluetooth!");
  delay(500);

  while (!Serial);
  // LoRa
  Serial.println("LoRa Receiver");
  SerialBT.println("LoRa Receiver");

  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);

  //replace the LoRa.begin(---E-) argument with your location's frequency
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America
  while (!LoRa.begin(866E6)) {
    Serial.print(".");
    SerialBT.print(".");
    delay(500);
  }
   // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
  SerialBT.println("LoRa Initializing OK!");
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet ");
    SerialBT.print("Received packet ");

    // read packet
    while (LoRa.available()) {
      String LoRaData = LoRa.readString();
      Serial.print(LoRaData); 
      SerialBT.print(LoRaData); 
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    SerialBT.print("' with RSSI ");

    Serial.println(LoRa.packetRssi());
    SerialBT.println(LoRa.packetRssi());
  }
}

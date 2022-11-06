/*********
  Xavier Charles
  Based on 
  Complete project details at http://randomnerdtutorials.com  
  and 
  https://electronza.com/gardena-water-timer-controller-arduino-uno/#prettyPhoto
  Using H-bride L9110 but just half of it as it was designed to control 2 motors.
  In order to change power signed I will use only  the Motor A pins
*********/

//Add bluetooth for logs
#include "BluetoothSerial.h"

// LoRa
#include <SPI.h>
#include <LoRa.h>

// LoRa - define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

// END LoRa

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

RTC_DATA_ATTR int loraCount = 0;

void sendDataWithLoRa(){
  Serial.print("Sending packet: ");
  SerialBT.print("Sending packet: ");
  Serial.println(loraCount);
  SerialBT.println(loraCount);

  LoRa.beginPacket();

  LoRa.print("hello");
  Serial.print("Hello: ");
  SerialBT.print("Hello: ");

  LoRa.print(loraCount);
  Serial.println(loraCount);
  SerialBT.println(loraCount);

  LoRa.endPacket();
  Serial.println("EndPacket");
  SerialBT.println("EndPacket");

}

void setup() {

  Serial.begin(115200);
  SerialBT.begin("ESP32BT");

  // LoRa
  Serial.println("LoRa Sender");
  SerialBT.println("LoRa Sender");

  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);

  //replace the LoRa.begin(---E-) argument with your location's frequency
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America
  //unsigned long previousMillis = 0;
  //const long interval = 1000;
  //  unsigned long currentMillis = millis();
  while (!LoRa.begin(866E6)) {
    Serial.print(".");
    SerialBT.print(".");
   //if (currentMillis - previousMillis >= interval) {
   // // save the last time you blinked the LED
   // previousMillis = currentMillis;
   //}
   delay(500);
  }
   // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
  SerialBT.println("LoRa Initializing OK!");
  // END LoRa

}

void loop() {
  sendDataWithLoRa();
  loraCount++;
  Serial.print("Next count value: ");
  SerialBT.print("Next count value: ");
  Serial.println(loraCount);
  SerialBT.println(loraCount);
  delay(1000);
}

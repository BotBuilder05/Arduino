/*********
  Modified from the examples of the Arduino LoRa library
  More resources: https://randomnerdtutorials.com
  DHT : https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-sensor-arduino-ide/
*********/

//Add bluetooth for logs
#include "BluetoothSerial.h"
#include <SPI.h>
#include <LoRa.h>
#include "DHT.h"

//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2
#define DHTPIN 15 // Digital pin connected to the DHT sensor

#define DHTTYPE DHT11   // DHT 11

BluetoothSerial SerialBT;
int counter = 0;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  SerialBT.begin("ESP32BT");
  while (!Serial);
  Serial.println("LoRa Sender");
  SerialBT.println("LoRa Sender");

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

  dht.begin();
  Serial.println("DHT Initializing OK!");
  SerialBT.println("DHT Initializing OK!");
}

void loop() {

  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  SerialBT.print(F("Temperature: "));
  SerialBT.print(t);
  SerialBT.print(F("°C "));

  Serial.print("Sending packet: ");
  SerialBT.print("Sending packet: ");
  Serial.println(counter);
  SerialBT.println(counter);

  //Send LoRa packet to receiver
  LoRa.beginPacket();
  //LoRa.print("hello ");
  //LoRa.print(counter);
  LoRa.print(t);
  //SerialBT.print("hello ");
  //SerialBT.println(counter);
  SerialBT.println(t);
  LoRa.endPacket();

  counter++;
  if  (counter == 20000) {
      SerialBT.println("Reset counter");
      Serial.println("Reset counter");
      counter = 0;
  }

  delay(10000);
}


/*
 * https://circuits4you.com
 * ESP32 LED Blink Example
 * Board ESP23 DEVKIT V1
 * 
 * ON Board LED GPIO 2
 */
// 4/23 marche
#define LED 22


void setup() {
  // Set pin mode
  pinMode(LED,OUTPUT);
}

void loop() {
  delay(500);
  digitalWrite(LED,HIGH);
  delay(500);
  digitalWrite(LED,LOW);
}

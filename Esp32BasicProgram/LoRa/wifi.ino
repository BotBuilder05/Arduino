/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-client-server-wi-fi/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <WiFi.h>

const char* ssid = "velours";
const char* password = "valentineaudreyxavier";
//const char* ssid = "TuxAlp2";
//const char* password = "zomu6637";

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  
    if(WiFi.status()== WL_CONNECTED ){ 
      Serial.println("Wifi Connected!!!");
      delay (6000);
    }
    else {
      Serial.println("WiFi Disconnected");
      delay(500);
    }
}

#include "WiFi.h" // Enables the ESP32 to connect to the local network (via WiFi)
#include "FS.h"
#include "WebServer.h" // Enable WebServer

const char* ssid = "velours";                 // Your personal network SSID
const char* password = "valentineaudreyxavier"; // Your personal network password

WebServer server(80); 
 
// HTML & CSS contents which display on web server
String HTML = "<!DOCTYPE html>\
<html>\
<body>\
<h1>My First Web Server with ESP32 - Station Mode &#128522;</h1>\
</body>\
</html>";


// Handle root url (/)
void handle_root() {
  Serial.println("Handle Root");

  server.send(200, "text/html", HTML);
}

void setup() {
    // Connect to your wi-fi modem
  WiFi.begin(ssid, password);

  // Check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  Serial.begin(115200);
  Serial.println("");
  Serial.println("WiFi connected successfully");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());  //Show ESP32 IP on serial

  server.on("/", handle_root);

  server.begin();
  Serial.println("HTTP server started");
  delay(100); 
}

void loop() {
  Serial.println("Loop");
  server.handleClient();
  delay(1);
  Serial.println("End - Loop ");
}


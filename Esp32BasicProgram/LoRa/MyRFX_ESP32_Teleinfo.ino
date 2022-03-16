
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <SPI.h>
#include <Wire.h>
#include "SSD1306.h"
#include "OLEDDisplayUi.h"
#include "esp_system.h"

/************************* WiFi Access Point *********************************/

const char* ssid = "*******";
const char* password = "*******";
/************************* Adafruit.io Setup *********************************/


const char* mqttServer = "192.168.0.90";
const int mqttPort = 1883;
const char* mqttUser = "****";
const char* mqttPassword = "****";

const char* mqttTopic = "domoticz/in";
/************ Global State (you don't need to change this!) ******************/

WiFiClient espClient;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
PubSubClient client(espClient);

SSD1306 display(0x3c, 5, 4);

long lastMsg = 0;
char msg[128];
int value = 0;

int debug = 1;

int idx_kwh_msg = 4376; //4376 Ã  renseigner, l'idx de l'objet Domoticz Ã  mettre Ã  jour
int idx_papp_msg = 4375;//4375
int idx_current_msg = 4372; //4372
int idx_vcurrent_max_msg = 4373;//4373
int idx_isousc_msg = 4374;//4374
int idx_index_HC_msg = 0;
int idx_index_HP_msg = 0;

int idx_edf_compteur = 4377;//4377 4379 pour test

long delai_TX_mqtt = 10000;
long mem_Timer;
bool TX_mqtt;
int index_MQTT;
int index_oled;

long delai_Affichage_OLED = 3000;
long mem_delai_Affichage_OLED;


//==============================================================
// Téléinfo EDF
//==============================================================
HardwareSerial Serial_Teleinfo(2);
#define Serial_Teleinfo_RXPIN 16
#define Serial_Teleinfo_TXPIN 17

long Base, Base_old = 0;
byte Isousc, Isousc_old = 0;
long Index_HC, Index_HC_old = 0;
long Index_HP, Index_HP_old = 0;
byte I_A, I_A_old = 0;
int  P_W, P_W_old = 0;
int  Imax, Imax_old = 0;
float f_base = 0;



int Donnee = 0;

char charIn_Trame_Teleinfo = 0; // stock chaque charactÃ¨re recu de la trame teleinfo

String Ligne;      // la ligne complette (entre LF(0x0A) et CR(0x0D))
String Etiquette;  // le nom de la mesure via teleinfo
String Valeur;     // la valeur de la mesure
char Checksum;     // pour controle de la validit d des donnees recues
char Controle = 0;

void ConnectMQTT() {

  client.setServer(mqttServer, mqttPort);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP32Client", mqttUser, mqttPassword )) {
      Serial.println("connected")      ;
      display.clear();
      display.drawString(64, 1, "MQTT");
      display.drawString(64, 24,"Connecté !");
      display.display();
      delay(3000);
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      display.clear();
      display.drawString(64, 1, "MQTT");
      display.drawString(64, 24,"Erreur ..");
      display.display();
      delay(3000);
    }
  }
}

void publishDomoticz() {

  char mqttmsg[128];
  String chaine = "";

  index_MQTT++;
  if (index_MQTT > 5) {
    index_MQTT = 0;
  }

  switch (index_MQTT) {

      Serial.println(index_MQTT);

    case 1:
      chaine = "{\"idx\" : " + String(idx_edf_compteur) + ",\"nvalue\" : 0,\"svalue\" : \"" +  String(P_W) + ";" + String(f_base) + "\"}";
      chaine.toCharArray(mqttmsg, chaine.length() + 1);
      client.publish( mqttTopic, mqttmsg);
      Serial.print(mqttTopic);
      Serial.println(mqttmsg);
      break;

    case 2:
      chaine = "{\"idx\" : " + String(idx_papp_msg) + ",\"nvalue\" : 0,\"svalue\" : \"" +  String(f_base / 1000) + "\"}";
      chaine.toCharArray(mqttmsg, chaine.length() + 1);
      client.publish( mqttTopic, mqttmsg);
      Serial.print(mqttTopic);
      Serial.println(mqttmsg);

      break;

    case 3:
      chaine = "{\"idx\" : " + String(idx_kwh_msg) + ",\"nvalue\" : 0,\"svalue\" : \"" +  String(P_W ) + "\"}";
      chaine.toCharArray(mqttmsg, chaine.length() + 1);
      client.publish(mqttTopic, mqttmsg);
      Serial.print(mqttTopic);
      Serial.println(mqttmsg);
      break;

    case 4:
      chaine = "{\"idx\" : " + String(idx_current_msg) + ",\"nvalue\" : 0,\"svalue\" : \"" +  String(I_A ) + "\"}";
      chaine.toCharArray(mqttmsg, chaine.length() + 1);
      client.publish(mqttTopic, mqttmsg);
      Serial.print(mqttTopic);
      Serial.println(mqttmsg);
      break;

    case 5:
      chaine = "{\"idx\" : " + String(idx_isousc_msg) + ",\"nvalue\" : 0,\"svalue\" : \"" +  String(Isousc ) + "\"}";
      chaine.toCharArray(mqttmsg, chaine.length() + 1);
      client.publish(mqttTopic, mqttmsg);
      Serial.print(mqttTopic);
      Serial.println(mqttmsg);
      break;

    default:
      break;

  }

}
void Recupere_la_Teleinfo() {

  /*
     Information en provenance de la tlinfo
     ADCO 040422040644 5          (NÂ° d'identification du compteur : ADCO (12 caractÃ¨res))
     OPTARIF HC.. <               (Option tarifaire (type d'abonnement) : OPTARIF (4 car.))
     ISOUSC 45 ?                  (IntensitÃ© souscrite : ISOUSC ( 2 car. unitÃ© = ampÃ¨res))
     HCHC 077089461 0             (Index heures creuses si option = heures creuses : HCHC ( 9 car. unitÃ© = Wh))
     HCHP 096066754 >             (Index heures pleines si option = heures creuses : HCHP ( 9 car. unitÃ© = Wh))
     PTEC HP..                    (PÃ©riode tarifaire en cours : PTEC ( 4 car.))
     IINST 002 Y                  (IntensitÃ© instantanÃ©e : IINST ( 3 car. unitÃ© = ampÃ¨res))
     IMAX 044 G                   (IntensitÃ© maximale : IMAX ( 3 car. unitÃ© = ampÃ¨res))
     PAPP 00460 +                 (Puissance apparente : PAPP ( 5 car. unitÃ© = Volt.ampÃ¨res))
     HHPHC E 0                    (Groupe horaire si option = heures creuses ou tempo : HHPHC (1 car.))
     MOTDETAT 000000 B            (Mot d'Ã©tat (autocontrÃ´le) : MOTDETAT (6 car.))
  */


  //===========================================================
  // Une ligne de donnee est composee de la facon suivante :
  // 0x0A Etiquette 0X20 Valeur 0X20 Checksum 0X0D
  //===========================================================

  if (Serial_Teleinfo.available() > 0) {

    charIn_Trame_Teleinfo = Serial_Teleinfo.read() & 0x7F ;

    switch (charIn_Trame_Teleinfo) {

      case 0x0A:
        //debut d'une donnee 0X0A
        if (debug == 1) {
          Serial.print(Etiquette); Serial.print("/"); Serial.print(Valeur); Serial.print("/"); Serial.println(Checksum);
        }
        Etiquette = "";
        Donnee = 1;
        break;

      case 0x20:
        // detection d'un espace (deux dans une trame ) 0X20
        Donnee++;
        if (Donnee == 2) {
          Valeur = "";
        }

        if (Donnee == 3) {
          Checksum = 0;
        }
        break;

      case 0x0D:
        // detection de la fin de ligne 0x0D
        Controle = 0;

        // calcul du controle
        Ligne = Etiquette + " " + Valeur;
        for (byte i = 0; i < (Ligne.length()); i++) {
          Controle += Ligne[i];
        }
        //calcul du Checksum pour le controle
        Controle = (Controle & 0x3F) + 0x20;
        Donnee = 0;

        // si le controle est egal au checksum affection des valeurs
        if (Controle == Checksum) {

          //Affection des valeurs en fonction des etiquettes
          // transmission des valeurs soit sur changement de valeur (ex: comparaison avec la memoire Base_old et Base
          // ou périodiquement avec la fonction millis() , pas c'est pas trés précis....
          // régler à  votre convenance la variable delai_TX_mqtt

          // format de la trame pour Domoticz MQTT /in
          //{"idx" : 7,"nvalue" : 0,"svalue" : "90;2975.00"}

          if (Etiquette.substring (0, 4) == "BASE") {
            Base = Valeur.toFloat(); Serial.print(" => Consommation : "); Serial.print(Base); Serial.println(" Wh");
            f_base = float(Base);
          }

          if (Etiquette.substring (0, 4) == "PAPP") {
            P_W = Valeur.toInt(); Serial.print(" => Puissance apparente : "); Serial.print(P_W); Serial.println(" W");
          }

          if (Etiquette.substring (0, 4) == "IINS") {
            I_A = Valeur.toInt(); Serial.print(" => Intensite instantanee : "); Serial.print(I_A); Serial.println(" A");
          }

          if (Etiquette.substring (0, 4) == "IMAX") {
            Imax = Valeur.toInt(); Serial.print(" => I.Maximum : "); Serial.print(Imax); Serial.println(" A");
          }

          if (Etiquette.substring (0, 6) == "ISOUSC") {
            Isousc = Valeur.toInt(); Serial.print(" => I.Souscrit : "); Serial.print(Isousc); Serial.println(" A");
          }

          /*
                               if (Etiquette.substring (0,4)=="HCHC") {
                                   Index_HC = Valeur.toFloat();Serial.print(" => Index Heures creuses : ");Serial.print(Index_HC);Serial.println(" Wh");

                               if (Etiquette.substring (0,4)=="HCHP") {
                                   Index_HP = Valeur.toFloat();Serial.print(" => Index Heures pleines : ");Serial.print(Index_HP);Serial.println(" Wh");
          */
          break;

        default:
          // prise en compte des caractres significatifs

          if (Donnee == 1) {
            Etiquette = Etiquette + charIn_Trame_Teleinfo;
          }

          if (Donnee == 2) {
            Valeur = Valeur + charIn_Trame_Teleinfo;
          }

          if (Donnee == 3) {
            Checksum = Checksum + charIn_Trame_Teleinfo;
          }
        }
    }
  }
}
void drawTextFlowDemo() {
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawStringMaxWidth(0, 0, 128,
  "Lorem ipsum\n dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore." );
}

void setup() { 
  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_24);
    
  Serial.begin(115200);
  delay(10);
  
  Serial_Teleinfo.begin(1200);
  delay(10);
  
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    display.clear();
    display.drawString(64, 1, "WIFI");
    display.drawString(64, 24,"Erreur ..");
    display.display();
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname("ESP32 Teleinfo");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  ConnectMQTT();


}

void affichageOled() {
  char oledmessage[128];
  String chaine = "";

  index_oled++;
  if (index_oled > 4) {
    index_oled = 0;
  }

  switch (index_oled) {
    
    case 1:
      display.clear();
      display.drawString(64, 1, String(P_W));
      display.drawString(64, 24, "Watts");
      display.display();
      break;

    case 2:
      display.clear();
      display.drawString(64, 1, String(f_base/1000));
      display.drawString(64, 24, "Kw/h");
      display.display();
      break;
      
    case 3:
      display.clear();
      display.drawString(64, 1, String(I_A));
      display.drawString(64, 24, "Ampères");
      display.display();
      break;

    case 4:
      display.clear();
      display.drawString(64, 1, String(Isousc));
      display.drawString(64, 24, "A souscrit");
      display.display();
    break;
    
    default:
      break;
  }
}

void loop() {
  
  client.loop();
  ArduinoOTA.handle();
  Recupere_la_Teleinfo() ;

    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    display.clear();
    display.drawString(64, 1, "WIFI");
    display.drawString(64, 24,"Erreur ..");
    display.display();
    delay(5000);
    ESP.restart();
  }

  if (client.connected()) {
    // Now we can publish stuff!
    if (millis() - mem_Timer > delai_TX_mqtt) {
      publishDomoticz();
      mem_Timer = millis();
    }
  }
  else
  {
       Serial.println("MQTT Failed! Rebooting...");
    display.clear();
    display.drawString(64, 1, "MQTT");
    display.drawString(64, 24,"Erreur ..");
    display.display();
    delay(5000);
    ESP.restart(); 
  }
    if (millis() - mem_delai_Affichage_OLED > delai_Affichage_OLED) {
      affichageOled();
      mem_delai_Affichage_OLED = millis();
   }
}


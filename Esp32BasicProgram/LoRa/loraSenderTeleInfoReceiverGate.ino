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

#define GATE 22

BluetoothSerial SerialBT;
//int counter = 0;

DHT dht(DHTPIN, DHTTYPE);

// EDF variable
int index_MQTT;

int idx_kwh_msg = 4376; //4376 Ã  renseigner, l'idx de l'objet Domoticz Ã  mettre Ã  jour
int idx_papp_msg = 4375;//4375
int idx_current_msg = 4372; //4372
int idx_vcurrent_max_msg = 4373;//4373
int idx_isousc_msg = 4374;//4374
int idx_index_HC_msg = 0;
int idx_index_HP_msg = 0;

int idx_edf_compteur = 88888;//4377 4379 pour test

//==============================================================
// Téléinfo EDF
//==============================================================
int debug = 1;

HardwareSerial Serial_Teleinfo(2);
//#define Serial_Teleinfo_RXPIN 16
//#define Serial_Teleinfo_TXPIN 17
#define Serial_Teleinfo_RXPIN 3
#define Serial_Teleinfo_TXPIN 64 //not used 
//#define Serial_Teleinfo_TXPIN 17

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

void Recupere_la_Teleinfo() {

  /*
     Information en provenance de la tlinfo
     ADCO 040422040644 5          (N° d'identification du compteur : ADCO (12 caracteres))
     OPTARIF HC.. <               (Option tarifaire (type d'abonnement) : OPTARIF (4 car.))
     ISOUSC 45 ?                  (Intensite souscrite : ISOUSC ( 2 car. unite = amperes))
     HCHC 077089461 0             (Index heures creuses si option = heures creuses : HCHC ( 9 car. unite = Wh))
     HCHP 096066754 >             (Index heures pleines si option = heures creuses : HCHP ( 9 car. unite = Wh))
     PTEC HP..                    (Periode tarifaire en cours : PTEC ( 4 car.))
     IINST 002 Y                  (Intensite instantane : IINST ( 3 car. unite = amperes))
     IMAX 044 G                   (Intensite maximale : IMAX ( 3 car. unite = amperes))
     PAPP 00460 +                 (Puissance apparente : PAPP ( 5 car. unite = Volt.amperes))
   N  HHPHC E 0                    (Groupe horaire si option = heures creuses ou tempo : HHPHC (1 car.))
   N  MOTDETAT 000000 B            (Mot d'etat (autocontrole) : MOTDETAT (6 car.))
  */


  //===========================================================
  // Une ligne de donnee est composee de la facon suivante :
  // 0x0A Etiquette 0X20 Valeur 0X20 Checksum 0X0D
  //===========================================================

  //debug
  Serial.println("On est ds teleinfo.");

  if (Serial_Teleinfo.available() > 0) {
  //debug
  Serial.println("On a une Teleinfo available.");

    charIn_Trame_Teleinfo = Serial_Teleinfo.read() & 0x7F ;
    // debug
    Serial.print("On a lu une Teleinfo : ");
    Serial.println(charIn_Trame_Teleinfo);


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
  else {
      Serial.print("Serial_Teleinfo.available: ");
      Serial.println(Serial_Teleinfo.available());
  }
}
/*
EXAMPLE DE TRAME

            ADCO 524563565245 /
            OPTARIF HC.. <
4374 Isousc ISOUSC 20 8
            HCHC 001065963 _
            HCHP 001521211 '
            PTEC HC.. S
     I_A    IINST 001 I
     Imax   IMAX 008 2
            PMAX 06030 3
4375 P_W    PAPP 01250 +
            HHPHC E 0
            MOTDETAT 000000 B
            PPOT 00 #

            ADCO 524563565245 /
            OPTARIF HC.. <
4374 Isousc ISOUSC 20 8

int idx_kwh_msg = 4376; //4376 Ã  renseigner, l'idx de l'objet Domoticz Ã  mettre Ã  jour
int idx_papp_msg = 4375;//4375
int idx_current_msg = 4372; //4372
int idx_vcurrent_max_msg = 4373;//4373
int idx_index_HC_msg = 0;
int idx_index_HP_msg = 0;

int idx_edf_compteur = 88888;//4377 4379 pour test

Base = Valeur.toFloat(); Serial.print(" => Consommation : "); Serial.print(Base); Serial.println(" Wh");
P_W = Valeur.toInt(); Serial.print(" => Puissance apparente : "); Serial.print(P_W); Serial.println(" W");
I_A = Valeur.toInt(); Serial.print(" => Intensite instantanee : "); Serial.print(I_A); Serial.println(" A");
Imax = Valeur.toInt(); Serial.print(" => I.Maximum : "); Serial.print(Imax); Serial.println(" A");
Isousc = Valeur.toInt(); Serial.print(" => I.Souscrit : "); Serial.print(Isousc); Serial.println(" A");
Index_HC = Valeur.toFloat();Serial.print(" => Index Heures creuses : ");Serial.print(Index_HC);Serial.println(" Wh");
Index_HP = Valeur.toFloat();Serial.print(" => Index Heures pleines : ");Serial.print(Index_HP);Serial.println(" Wh");

*/
void sendFrame() {

  char mqttmsg[128];
  String chaine = "";

  index_MQTT++;
  if (index_MQTT > 5) {
    index_MQTT = 0;
  }

  switch (index_MQTT) {

//  Serial.println(index_MQTT);

    case 1:
      chaine = "{\"idx\" : " + String(idx_edf_compteur) + ",\"nvalue\" : 0,\"svalue\" : \"" +  String(P_W) + ";" + String(f_base) + "\"}";
      chaine.toCharArray(mqttmsg, chaine.length() + 1);
//      client.publish( mqttTopic, mqttmsg);
//      Serial.print(mqttTopic);
      Serial.println(mqttmsg);
//      SerialBT.println(mqttmsg);
      LoRa.print(mqttmsg);
      break;

    case 2:
      chaine = "{\"idx\" : " + String(idx_papp_msg) + ",\"nvalue\" : 0,\"svalue\" : \"" +  String(f_base / 1000) + "\"}";
      chaine.toCharArray(mqttmsg, chaine.length() + 1);
//      client.publish( mqttTopic, mqttmsg);
//      Serial.print(mqttTopic);
//      Serial.println(mqttmsg);
      LoRa.print(mqttmsg);

      break;

    case 3:
      chaine = "{\"idx\" : " + String(idx_kwh_msg) + ",\"nvalue\" : 0,\"svalue\" : \"" +  String(P_W ) + "\"}";
      chaine.toCharArray(mqttmsg, chaine.length() + 1);
//      client.publish(mqttTopic, mqttmsg);
//      Serial.print(mqttTopic);
//      Serial.println(mqttmsg);
      LoRa.print(mqttmsg);
      break;

    case 4:
      chaine = "{\"idx\" : " + String(idx_current_msg) + ",\"nvalue\" : 0,\"svalue\" : \"" +  String(I_A ) + "\"}";
      chaine.toCharArray(mqttmsg, chaine.length() + 1);
//      client.publish(mqttTopic, mqttmsg);
//      Serial.print(mqttTopic);
//      Serial.println(mqttmsg);
      LoRa.print(mqttmsg);
      break;

    case 5:
      chaine = "{\"idx\" : " + String(idx_isousc_msg) + ",\"nvalue\" : 0,\"svalue\" : \"" +  String(Isousc ) + "\"}";
      chaine.toCharArray(mqttmsg, chaine.length() + 1);
//      client.publish(mqttTopic, mqttmsg);
//      Serial.print(mqttTopic);
//      Serial.println(mqttmsg);
      LoRa.print(mqttmsg);
      break;

    default:
      break;

  }
}

void setup() {
  // Set pin mode
  pinMode(GATE,OUTPUT);

  //initialize Serial Monitor
  Serial.begin(115200);
  SerialBT.begin("ESP32BT");
  while (!Serial);
  Serial.println("LoRa Sender");
  SerialBT.println("LoRa Sender");

  Serial_Teleinfo.begin(1200);

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
//  Serial.println("LoRa Initializing OK!");
//  SerialBT.println("LoRa Initializing OK!");

  dht.begin();
//  Serial.println("DHT Initializing OK!");
//  SerialBT.println("DHT Initializing OK!");
}

void driveGate(){
  digitalWrite(GATE,HIGH);
  delay(500);
  digitalWrite(GATE,LOW);
}

void manageGateSignal() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  String LoRaData = "";
  Serial.print(".");
  //Serial.println(packetSize);
  if (packetSize) {
    // received a packet
    Serial.print("Received packet ");
    }

    // read packet
    while (LoRa.available()) {
      //String LoRaData = LoRa.readString();
      LoRaData = LoRa.readString();
      //LoRaData = "DATA";
      Serial.print("Data: "); 
      Serial.println(LoRaData); 
    }
    if (LoRaData){
      driveGate();
    }
  }
  else {
      LoRaData = "NO_DATA";
  }
}


void loop() {

  // Wait a few seconds between measurements.
  Recupere_la_Teleinfo();
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
  Serial.println(F("°C "));
//  SerialBT.print(F("Temperature: "));
//  SerialBT.print(t);
//  SerialBT.print(F("°C "));

//  Serial.print("Sending packet: ");
//  SerialBT.print("Sending packet: ");
//  Serial.println(counter);
//  SerialBT.println(counter);

  //Send LoRa packet to receiver
  LoRa.beginPacket();
  //LoRa.print("hello ");
  //LoRa.print(counter);
  LoRa.print(t);
  sendFrame();
  //SerialBT.print("hello ");
  //SerialBT.println(counter);
  //SerialBT.println(t);
  LoRa.endPacket();

//  counter++;
//  if  (counter == 20000) {
//      SerialBT.println("Reset counter");
//      Serial.println("Reset counter");
//      counter = 0;
//  }

  manageGateSignal();

  delay(10000);
}


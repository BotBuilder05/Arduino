/*********
  Modified from the examples of the Arduino LoRa library
  More resources: https://randomnerdtutorials.com
  DHT : https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-sensor-arduino-ide/
*********/
#include "BluetoothSerial.h"
#include <SPI.h>
#include <LoRa.h>
#include "DHT.h"

#include <LibTeleinfo.h>

//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2
#define DHTPIN 15 // Digital pin connected to the DHT sensor

#define DHTTYPE DHT11   // DHT 11

// NEW TELEINFO
#define PUSH_BUTTON     0

#define TIC_ENABLE_PIN  4
#define TIC_RX_PIN      16
#define TIC_TX_PIN      17

#define RGB_LED_PIN     2

// END NEW TELEINFO


BluetoothSerial SerialBT;
//int counter = 0;

DHT dht(DHTPIN, DHTTYPE);

// New TeleInfo
TInfo tinfo; // Teleinfo object

// Pour clignotement LED asynchrone
unsigned long blinkLed  = 0;
boolean tick1sec=0;// one for interrupt, don't mess with
unsigned long uptime=0; // save value we can use in sketch even if we're interrupted
uint8_t blinkDelay= 0;

// Used to indicate if we need to send all date or just modified ones
boolean fulldata = true;

/* ======================================================================
Function: ADPSCallback 
Purpose : called by library when we detected a ADPS on any phased
Input   : phase number 
            0 for ADPS (monophase)
            1 for ADIR1 triphase
            2 for ADIR2 triphase
            3 for ADIR3 triphase
Output  : - 
Comments: should have been initialised in the main sketch with a
          tinfo.attachADPSCallback(ADPSCallback())
====================================================================== */
void ADPSCallback(uint8_t phase)
{
  // Envoyer JSON { "ADPS"; n}
  // n = numero de la phase 1 à 3
  if (phase == 0)
    phase = 1;
  Serial.print(F("{\"ADPS\":"));
  Serial.print('0' + phase);
  Serial.println(F("}"));
}

/* ======================================================================
Function: sendJSON
Purpose : dump teleinfo values on serial
Input   : linked list pointer on the concerned data
          true to dump all values, false for only modified ones
Output  : -
Comments: -
====================================================================== */
void sendJSON(ValueList * me, boolean all)
{
  bool firstdata = true;
  char mqttmsg[128];
  String chaine = "";

  // DEBUG
  Serial.println("We are in sendJSON");
  // Got at least one ?
  if (me) {
    // Json start
    Serial.print(F("{"));
    chaine="{";

    if (all) {
      Serial.print(F("\"_UPTIME\":"));
      chaine=chaine+"\"_UPTIME\":";
      Serial.print(uptime, DEC);
      chaine=chaine+uptime, DEC;
      firstdata = false;
    }

    // Loop thru the node
    while (me->next) {
      // go to next node
      me = me->next;

      // uniquement sur les nouvelles valeurs ou celles modifiées
      // sauf si explicitement demandé toutes
      if ( all || ( me->flags & (TINFO_FLAGS_UPDATED | TINFO_FLAGS_ADDED) ) )
      {
        // First elemement, no comma
        if (firstdata)
          firstdata = false;
        else
          Serial.print(F(", ")) ;
          chaine=chaine+", ";

        Serial.print(F("\"")) ;
        chaine=chaine+"\"";
        Serial.print(me->name) ;
        chaine=chaine+me->name;
        Serial.print(F("\":")) ;
        chaine=chaine+"\":";

        // we have at least something ?
        if (me->value && strlen(me->value))
        {
          boolean isNumber = true;
          char * p = me->value;

          // check if value is number
          while (*p && isNumber) {
            if ( *p < '0' || *p > '9' )
              isNumber = false;
            p++;
          }

          // this will add "" on not number values
          if (!isNumber) {
            Serial.print(F("\"")) ;
            chaine=chaine+"\"";
            Serial.print(me->value) ;
            chaine=chaine+me->value;
            Serial.print(F("\"")) ;
            chaine=chaine+"\"";
          }
          // this will remove leading zero on numbers
          else
            Serial.print(atol(me->value));
            //chaine=chaine+atol(me->value);
        }
      }
    }
   // Json end
   Serial.println(F("}")) ;
   Serial.println(chaine) ;
   LoRa.print(chaine);
  }
}

/* ======================================================================
Function: NewFrame 
Purpose : callback when we received a complete teleinfo frame
Input   : linked list pointer on the concerned data
Output  : - 
Comments: -
====================================================================== */
void NewFrame(ValueList * me)
{
  // Start short led blink
  #ifdef LED_RED_PIN
  //digitalWrite(LED_RED_PIN, LOW);
  #endif
  #ifdef RGB_LED_PIN
////  strip.SetPixelColor(0, red);
////  strip.Show();
  #endif
  blinkLed = millis();
  blinkDelay = 50; // 50ms
  
  // DEBUG
  Serial.println("NewFrame");
  // Envoyer les valeurs uniquement si demandé
  if (fulldata)
    sendJSON(me, true);

  fulldata = false;
}

/* ======================================================================
Function: UpdatedFrame 
Purpose : callback when we received a complete teleinfo frame
Input   : linked list pointer on the concerned data
Output  : - 
Comments: it's called only if one data in the frame is different than
          the previous frame
====================================================================== */
void UpdatedFrame(ValueList * me)
{
  // Start long led blink
  #ifdef LED_BLU_PIN
  //digitalWrite(LED_BLU_PIN, LOW);
  #endif
  #ifdef RGB_LED_PIN
////  strip.SetPixelColor(0, blue);
////  strip.Show();
  #endif

  blinkLed = millis();
  blinkDelay = 50; // 50ms

  // DEBUG
  Serial.println("UpdatedFrame");
  // Envoyer les valeurs 
  sendJSON(me, fulldata);
  fulldata = false;
}
// End - New TeleInfo


/*====================================================================== 
SETUP

====================================================================== */
void setup() {
  // Init DHT Sensor
  dht.begin();

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

// NEW TELEINFO
  // Serial, pour le debug
  // Already done above Serial.begin(115200);
  Serial.println(F("\r\n\r\n=============="));
  Serial.println(F("Teleinfo"));


  // Teleinfo enable pin
  #ifdef TIC_ENABLE_PIN
  pinMode(TIC_ENABLE_PIN, OUTPUT);
  digitalWrite(TIC_ENABLE_PIN, HIGH);
  Serial.printf_P(PSTR("Enable TIC on  GPIO%d\r\n"), TIC_ENABLE_PIN);
  #endif

  // Button
  #ifdef PUSH_BUTTON
  pinMode(PUSH_BUTTON, INPUT_PULLUP);
  Serial.printf_P(PSTR("Enable Button on GPIO=%d\r\n"), PUSH_BUTTON);
  #endif


  Serial.printf_P(PSTR("TIC RX = GPIO=%d\r\n"), TIC_RX_PIN);
  Serial1.begin(1200, SERIAL_7E1, TIC_RX_PIN);
  pinMode(TIC_RX_PIN, INPUT_PULLUP);

  // Init teleinfo
  tinfo.init();

  // Attacher les callback dont nous avons besoin
  // pour cette demo, ADPS et TRAME modifiée
  tinfo.attachADPS(ADPSCallback);
  tinfo.attachUpdatedFrame(UpdatedFrame);
  tinfo.attachNewFrame(NewFrame);
// END NEW TELEINFO
}

/*====================================================================== 
LOOP

====================================================================== */
void loop() {

  // Wait a few seconds between measurements.
  //delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // NEW TELEINFO
  static char c;
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  // END - NEWTELEINFO

  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  //Serial.print(F("Temperature: "));
  //Serial.print(t);
  //Serial.println(F("°C "));

  // NEW TELEINFO
  // Avons nous recu un ticker de seconde?
  if (tick1sec)
  {
    tick1sec = false;
    uptime++;

    // Forcer un envoi de trame complète toutes les minutes
    // fulldata sera remis à 0 après l'envoi
    if (uptime % 60 == 0)
      fulldata = true;
  }

// On a reçu un caractère ?
  if ( Serial1.available() ) {
    //  Serial.println("On a un serial1 available");
    // Le lire
    c = Serial1.read();

    // Gérer
    tinfo.process(c);

    // L'affcher dans la console
    if (c!=TINFO_STX && c!=TINFO_ETX) {
      Serial.print(c);
    }
  }
  //DEBUG
      //z=15;//z=c
      //Serial.print("Valeur de C: ");
      //Serial.print(c);
      //Serial.print();
      //Serial.println("Envoi de C via LoRa");
      //LoRa.print("hello ");
      //LoRa.print(counter);
      LoRa.beginPacket();
      LoRa.println(c);
      LoRa.endPacket();
  //END - NEW TELEINFO


  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.println(t);
  LoRa.endPacket();

  if (currentMillis - previousMillis > 1000 ) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;
    tick1sec = true;
  }
}

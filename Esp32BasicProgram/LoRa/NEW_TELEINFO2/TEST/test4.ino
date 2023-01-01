// **********************************************************************************
// Arduino Teleinfo sample, return JSON data of modified teleinfo values received
// This sketch works as is only for Denky D4 board, need to be adapted for other
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// http://creativecommons.org/licenses/by-sa/4.0/
//
// for detailled explanation of this library see dedicated article
// https://hallard.me/libteleinfo/
//
// For any explanation about teleinfo or use, see my blog
// https://hallard.me/category/tinfo
// 
// see dedicated article here https://hallard.me/demystifier-la-teleinfo/
//
// Written by Charles-Henri Hallard (https://hallard.me)
//
// History : V1.00 2022-10-25 - First release
//
// All text above must be included in any redistribution.
//
// **********************************************************************************
#include <LibTeleinfo.h>
////#include <NeoPixelBrightnessBus.h> 

#define SERIAL_DEBUG  Serial
#define SERIAL_TIC    Serial1

// RGB Led Pins
#define NEOPIXEL_DATA 14
#define NEOPIXEL_LEDS 1

#define PUSH_BUTTON 0

// Teleinfo RXD pin is connected to ESP32-PICO-V3-02 GPIO8
#define TIC_RX_PIN  16

#define COLOR_BLACK     RgbColor(0x00, 0x00, 0x00)
#define COLOR_RED       RgbColor(0xFF, 0x00, 0x00)
#define COLOR_ORANGE    RgbColor(0xFF, 0x22, 0x00)
#define COLOR_YELLOW    RgbColor(0xFF, 0xAA, 0x00)
#define COLOR_GREEN     RgbColor(0x00, 0xFF, 0x00)
#define COLOR_CYAN      RgbColor(0x00, 0xFF, 0xFF)
#define COLOR_BLUE      RgbColor(0x00, 0x00, 0xFF)
#define COLOR_VIOLET    RgbColor(0x99, 0x00, 0xFF)
#define COLOR_MAGENTA   RgbColor(0xFF, 0x00, 0x33)
#define COLOR_PINK      RgbColor(0xFF, 0x33, 0x77)
#define COLOR_AQUA      RgbColor(0x55, 0x7D, 0xFF)
#define COLOR_WHITE     RgbColor(0xFF, 0xFF, 0xFF)

// Led is common anode so reversed, this means
// 0   = Full brightness
// 255 = Minimal brightness
// 256 = OFF
uint8_t rgb_brightness = 255;

////NeoPixelBrightnessBus<NeoGrbFeature, NeoEsp32Rmt0Ws2812xMethod> strip( NEOPIXEL_LEDS, NEOPIXEL_DATA);

// Default mode, can be switched back and forth 
// with Denky D4 push button
_Mode_e tinfo_mode = TINFO_MODE_HISTORIQUE; 

TInfo tinfo; // Teleinfo object

// Pour clignotement LED asynchrone
unsigned long blinkLed  = 0;
int blinkDelay= 0;

// Uptime timer
boolean tick1sec=0;// one for interrupt, don't mess with 
unsigned long uptime=0; // save value we can use in sketch even if we're interrupted

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
////  strip.SetPixelColor(0, COLOR_RED);
////  strip.Show();
  blinkLed = millis();
  blinkDelay = 200; // 200ms
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
////  strip.SetPixelColor(0, COLOR_BLUE);
////  strip.Show();
  blinkLed = millis();
  blinkDelay = 200; // 200ms
}

/* ======================================================================
Function: initSerial
Purpose : Configure (or reconfigure Serial Port)
Input   : -
Output  : - 
Comments: -
====================================================================== */
void initSerial()
{
  // Cleanup
  SERIAL_TIC.flush();
  SERIAL_TIC.end();

  // Configure Teleinfo 
  SERIAL_DEBUG.printf_P(PSTR("TIC RX=GPIO%d  Mode:"), TIC_RX_PIN);
  SERIAL_TIC.begin(tinfo_mode == TINFO_MODE_HISTORIQUE ? 1200 : 9600, SERIAL_7E1, TIC_RX_PIN);

  if ( tinfo_mode == TINFO_MODE_HISTORIQUE ) {
    SERIAL_DEBUG.println(F("Historique"));
  } else {
    SERIAL_DEBUG.println(F("Standard"));
  }
}

/* ======================================================================
Function: ledOff
Purpose : Setup I/O for RGB Led to be OFF
Input   : -
Output  : - 
Comments: -
====================================================================== */
void ledOff() {
  // Can be one of the 2 option
////  strip.SetPixelColor(0, COLOR_BLACK);
////  strip.Show();
}

/* ======================================================================
Function: setup
Purpose : Setup I/O and other one time startup stuff
Input   : -
Output  : - 
Comments: -
====================================================================== */
void setup()
{
////  strip.SetBrightness(rgb_brightness);
////  strip.Begin();
////  strip.SetPixelColor(0, COLOR_RED);
////  strip.Show();
  
  // Arduino LED Off
  ledOff();

  // Serial, pour le debug
  SERIAL_DEBUG.begin(115200);
  SERIAL_DEBUG.println(F("\r\n\r\n"));
  SERIAL_DEBUG.println(F("====================================="));
  SERIAL_DEBUG.println(F("     Teleinfo passthru Denky D4"));
  SERIAL_DEBUG.println(F("====================================="));
  SERIAL_DEBUG.println(F("  You can change teleinfo mode from"));
  SERIAL_DEBUG.println(F("Historique to Standard with B0 button"));

  // Button
  pinMode(PUSH_BUTTON, INPUT_PULLUP);     

  // Init Serial Port
  initSerial();

  // Init teleinfo
  tinfo.init(tinfo_mode);

  // Attacher les callback dont nous avons besoin
  // pour cette demo, ADPS et TRAME modifiée
  tinfo.attachUpdatedFrame(UpdatedFrame);
  tinfo.attachNewFrame(NewFrame); 
}

/* ======================================================================
Function: loop
Purpose : infinite loop main code
Input   : -
Output  : - 
Comments: -
====================================================================== */
void loop()
{
  static char c;
  static unsigned long previousMillis = 0;
  static uint8_t buttonState = 0;
  static unsigned long lastDebounceTime = 0;  

  unsigned long currentMillis = millis();

  // Button to switch mode
  uint8_t button = digitalRead(PUSH_BUTTON);

  // New Press 
  if ( button==LOW && buttonState==0) {
    buttonState = 1; 
    lastDebounceTime = millis(); 

  // Pressed enought (debounced)
  } else if ( buttonState==1 && button==LOW && (millis()-lastDebounceTime)>50 ) {
    buttonState = 2; 
 
  // Release (no need debouce here)
  } else if ( buttonState==2 && button==HIGH ) {

    // Invert mode
    if ( tinfo_mode == TINFO_MODE_HISTORIQUE ) {
      tinfo_mode = TINFO_MODE_STANDARD;
    } else  {
      tinfo_mode = TINFO_MODE_HISTORIQUE;
    }

    // Start long led blink
////    strip.SetPixelColor(0, COLOR_GREEN);
////    strip.Show();
    blinkLed = millis();
    blinkDelay = 100; // 100ms

    // Init Serial Port
    initSerial();

    // Init teleinfo
    tinfo.init(tinfo_mode);

    lastDebounceTime = millis(); 
    buttonState = 0;
  } 

  
  // Avons nous recu un ticker de seconde?
  if (tick1sec) {
    tick1sec = false;
    uptime++;
  }
  
  // On a reçu un caractère ?
  if ( SERIAL_TIC.available() ) {
    // Le lire
    c = SERIAL_TIC.read();

    // Gérer
    tinfo.process(c);

    // L'affcher dans la console
    if (c==TINFO_STX) {
      SERIAL_DEBUG.print("<STX>");
    } else if (c==TINFO_ETX) {
      SERIAL_DEBUG.print("<ETX>");
    } else if (c==TINFO_HT) {
      SERIAL_DEBUG.print("<TAB>");
    } else {
      SERIAL_DEBUG.print(c);
    }
  }

  // Verifier si le clignotement LED doit s'arreter 
  if (blinkLed && ((millis()-blinkLed) >= blinkDelay)) {
      ledOff();
      blinkLed = 0;
  }

  if (currentMillis - previousMillis > 1000 ) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   
    tick1sec = true;
  }
}

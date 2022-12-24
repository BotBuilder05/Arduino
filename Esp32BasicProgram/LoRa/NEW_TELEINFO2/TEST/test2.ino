#include <SoftwareSerial.h>
#include <LibTeleinfo.h>


#define LED_PIN     25

SoftwareSerial Serial1(3,4);
TInfo tinfo; 

// Pour clignotement LED asynchrone
unsigned long blinkLed  = 0;
uint8_t       blinkDelay= 0;

void DataCallback(ValueList * me, uint8_t  flags)
{
  // compteur de secondes basique
  Serial.print(millis()/1000);
  Serial.print(F("\t"));

  if (flags & TINFO_FLAGS_ADDED) 
    Serial.print(F("NEW -> "));

  if (flags & TINFO_FLAGS_UPDATED)
    Serial.print(F("MAJ -> "));

  // Display values
  Serial.print(me->name);
  Serial.print("=");
  Serial.println(me->value);
}

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
  printUptime();

  // Monophasé
  if (phase == 0 ) {
    Serial.println(F("ADPS"));
  }
  else {
    Serial.print(F("ADPS PHASE #"));
    Serial.println('0' + phase);
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
  digitalWrite(LEDPIN, HIGH);
  blinkLed = millis();
  blinkDelay = 50; // 50ms

  // Show our not accurate second counter
  printUptime();
  Serial.println(F("FRAME -> SAME AS PREVIOUS"));
}

/* ======================================================================
Function: NewFrame
Purpose : callback when we received a complete teleinfo frame
Input   : linked list pointer on the concerned data
Output  : -
Comments: it's called only if one data in the frame is different than
          the previous frame
====================================================================== */
void UpdatedFrame(ValueList * me)
{
  // Start long led blink
  digitalWrite(LEDPIN, HIGH);
  blinkLed = millis();
  blinkDelay = 100; // 100ms

  // Show our not accurate second counter
  printUptime();
  Serial.println(F("FRAME -> UPDATED"));
}



void setup()
{
  // Configure Teleinfo Soft serial 
  Serial1.begin(1200);

  // Init teleinfo
  tinfo.init();

  // Attacher les callback dont nous avons besoin
  // pour cette demo, toutes
  tinfo.attachADPS(ADPSCallback);
  tinfo.attachData(DataCallback);
  tinfo.attachNewFrame(NewFrame);
  tinfo.attachUpdatedFrame(UpdatedFrame);
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
  // On a reçu un caractère ?
  if ( Serial1.available() )
    tinfo.process(Serial1.read());

  // Verifier si le clignotement LED doit s'arreter
  if (blinkLed && ((millis()-blinkLed) >= blinkDelay))
  {
    digitalWrite(LEDPIN, LOW);
    blinkLed = 0;
  }
}

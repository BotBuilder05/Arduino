#include <SoftwareSerial.h>
#include <LibTeleinfo.h>

SoftwareSerial Serial1(3,4);
TInfo tinfo; 

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

void setup()
{
  // Configure Teleinfo Soft serial 
  Serial1.begin(1200);

  // Init teleinfo
  tinfo.init();

  // Attacher la callback dont nous avons besoin
  tinfo.attachData(DataCallback);
}

void loop()
{
  if ( Serial1.available() )
    tinfo.process(Serial1.read());
}

/*
  AnalogReadSerial ESP32 ADC example

  Reads an analog input on ADC1_0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +3.3V and ground.

  https://circuits4you.com
*/

int const pin = 34;
//int const pin = 5;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
    pinMode(pin, INPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  float temp;
  float ratio;
  float avg=0;
  int sensorValue;
  for (int i=0;i<20;i++)
  {
      sensorValue = analogRead(pin);
      avg = avg + analogRead(pin);
/*      Serial.print(i);
      Serial.print("  ");
      */
      Serial.println(sensorValue);
/*      Serial.print("  ");
      Serial.println(avg);
*/
      delay(2000);        // delay in between reads for stability
  }
  avg=avg/20;
  // read the input on analog ADC1_0:
  ratio = (4095.0 - 3805.0)/19.6;
  temp = (4095.0 - avg)/ratio;
/*  Serial.print(avg);
  Serial.print("  ");
  Serial.print(ratio);
  Serial.print(" ");
*/
  Serial.print(temp);
//  Serial.print("  ");
//  Serial.println(sensorValue);

  //if ((sensorValue > 3770) and (sensorValue<3810)){
  //     Serial.println(21.7);
  //}
  // print out the value you read:
}

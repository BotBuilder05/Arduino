#include <Wire.h>
#include <VL53L0X.h>

#define ERROR_DIFF 8 // for detecting if the ennemy is on right or left

#define NB_SENSORS 2
#define SENSOR_FL 0
#define SENSOR_FR 1
uint8_t sensors_pin[NB_SENSORS] = {13, 8}; //table of enable of sensors
int read_sensors[NB_SENSORS];

//line detector
#define LINE_R_ECHO_PIN A6
#define LINE_R_SENSOR_PIN A2
#define LINE_F_ECHO_PIN 1
#define LINE_F_SENSOR_PIN A2


VL53L0X sensor_fl; //front left
VL53L0X sensor_fr; //front rigth

void setupSensors()
{
  Wire.begin();

  for(int i=0; i<NB_SENSORS; i++)
    pinMode(sensors_pin[i], OUTPUT);

  for(int i=0; i<NB_SENSORS; i++)
    digitalWrite(sensors_pin[i], LOW);

  delay(100);
  
  Serial.println("Setting sensor front right");
  digitalWrite(sensors_pin[SENSOR_FR], HIGH);
  delay(100);
  sensor_fr.init();
  delay(100);
  Serial.println("Overwriting i2c addr to 0x02");
  sensor_fr.setAddress(0x02);
  sensor_fr.setTimeout(500);
  sensor_fr.setMeasurementTimingBudget(20000);
  //sensor_fr.startContinuous();
  sensor_fr.readRangeSingleMillimeters();
  
  Serial.println("Setting sensor front left");
  digitalWrite(sensors_pin[SENSOR_FL], HIGH);
  delay(100);
  sensor_fl.init();
  delay(100);
  Serial.println("Overwriting i2c addr to 0x01");
  sensor_fl.setAddress(0x01);
  sensor_fl.setTimeout(500);
  sensor_fl.setMeasurementTimingBudget(20000);
  //sensor_fl.startContinuous();
  sensor_fl.readRangeSingleMillimeters();

}

void readLineSensors(){
  
}

void readAll(){
  //Serial.print("Read : ");
  read_sensors[SENSOR_FL] = sensor_fl.readRangeSingleMillimeters()/10;
  read_sensors[SENSOR_FR] = sensor_fr.readRangeSingleMillimeters()/10;
  readLineSensors();
  /*Serial.print(read_sensors[SENSOR_FL]);
  Serial.print(" | ");
  Serial.println(read_sensors[SENSOR_FR]);*/
}


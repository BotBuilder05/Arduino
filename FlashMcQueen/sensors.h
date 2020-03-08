#include <Wire.h>
#include <VL53L0X.h>

//##### LASER ######
#define ERROR_DIFF 4 // for detecting if the ennemy is on right or left

#define PARASIT_DIFF 4

#define MAX_VALUE 800

#define NB_SENSORS_LASER 3
uint8_t sensors_pin[NB_SENSORS_LASER] = {13, 8, 11}; //table of enable of sensors laser

//##### COMMON INFO ####
#define NB_SENSORS 6
volatile int read_sensors[NB_SENSORS], old_read[NB_SENSORS];
#define SENSOR_FL 0
#define SENSOR_FR 1
#define SENSOR_L 2
#define SENSOR_R 3
#define SENSOR_LINE_R 4
#define SENSOR_LINE_L 5

//##### line detector ####
#define LINE_R_SENSOR_PIN A2
#define LINE_L_SENSOR_PIN A3

uint8_t black_value = 500; //calibrate at the beginning 


VL53L0X sensor_fl; //front left
VL53L0X sensor_fr; //front rigth
VL53L0X sensor_l; //left

void readLineSensors(){
  read_sensors[SENSOR_LINE_R] = analogRead(LINE_R_SENSOR_PIN);
  read_sensors[SENSOR_LINE_L] = analogRead(LINE_L_SENSOR_PIN);
}

void setupSensors()
{
  Wire.begin();

  /*for(int i=0; i<NB_SENSORS_LASER; i++)
    pinMode(sensors_pin[i], OUTPUT);

  for(int i=0; i<NB_SENSORS_LASER; i++)
    digitalWrite(sensors_pin[i], LOW);

  delay(100);*/
  
  Serial.println("Setting sensor front left");
  pinMode(sensors_pin[SENSOR_FL], OUTPUT);
  digitalWrite(sensors_pin[SENSOR_FL], HIGH);
  delay(100);
  sensor_fl.init();
  delay(100);
  Serial.println("Overwriting i2c addr to 0x01");
  sensor_fl.setAddress(0x01);
  sensor_fl.setTimeout(500);
  //sensor_fl.setMeasurementTimingBudget(20000);
 sensor_fl.startContinuous();
 // sensor_fl.readRangeSingleMillimeters();
  
  Serial.println("Setting sensor left");
  pinMode(sensors_pin[SENSOR_L], OUTPUT);
  digitalWrite(sensors_pin[SENSOR_L], HIGH);
  delay(100);
  sensor_l.init();
  delay(100);
  Serial.println("Overwriting i2c addr to 0x03");
  sensor_l.setAddress(0x03);
  sensor_l.setTimeout(500);
  //sensor_l.setMeasurementTimingBudget(20000);
  sensor_l.startContinuous();
  //sensor_l.readRangeSingleMillimeters();
  read_sensors[SENSOR_L] = sensor_l.readRangeContinuousMillimeters()/10;
  
  Serial.println("Setting sensor front right");
  pinMode(sensors_pin[SENSOR_FR], OUTPUT);
  digitalWrite(sensors_pin[SENSOR_FR], HIGH);
  delay(100);
  sensor_fr.init();
  delay(100);
  Serial.println("Overwriting i2c addr to 0x02");
  sensor_fr.setAddress(0x02);
  sensor_fr.setTimeout(500);
  //sensor_fr.setMeasurementTimingBudget(50000);
  sensor_fr.startContinuous();
  //sensor_fr.readRangeSingleMillimeters();

  //calibrate line sensors
  readLineSensors();
  //black_value = ((read_sensors[SENSOR_LINE_R]+read_sensors[SENSOR_LINE_L])/2);
}

void readAll(){
  //Serial.print("Read : ");
  //old_read = read_sensors;
  read_sensors[SENSOR_FL] = sensor_fl.readRangeContinuousMillimeters()/10;
  read_sensors[SENSOR_FR] = sensor_fr.readRangeContinuousMillimeters()/10;
  read_sensors[SENSOR_L] = sensor_l.readRangeContinuousMillimeters()/10;
  read_sensors[SENSOR_LINE_R] = analogRead(LINE_R_SENSOR_PIN);
  read_sensors[SENSOR_LINE_L] = analogRead(LINE_L_SENSOR_PIN);

  }

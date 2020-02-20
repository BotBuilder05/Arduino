#include <ACROBOTIC_SSD1306.h>
#include "motors.h"
#include "sensors.h"


//state machine
#define INIT 1
#define START 2
#define SEARCH 3
#define ATTACK 4
#define FOLLOW_RIGHT 5
#define FOLLOW_LEFT 6
#define BACK 7
#define RUSH 8
#define STOP 9


uint8_t current_state = 0, next_state = 0, last_state = 0;
//####end state machine def####

//pin for microstart
uint8_t microstart = 12;

//constant
const int boost_distance = 15,
          detection_distance = 35,
          max_time_boost = 300;

int count_time_boost = 0, old;

uint8_t rotate_sens = RIGHT;

boolean started = false;

//##### functions #####

//timed function 
void routine () {
  if(started) {
    old_read[SENSOR_FL] = read_sensors[SENSOR_FL];
    old_read[SENSOR_FR] = read_sensors[SENSOR_FR];
    readAll();
    oled.setTextXY(0,0);
    oled.putNumber(read_sensors[SENSOR_FL]);
    oled.putString("            ");
    /*oled.putString(" : ");
    oled.putString(read_sensors[SENSOR_FR]);
    oled.putString(" : ");
    oled.putString(read_sensors[SENSOR_L]);
    oled.putString("   ");
    /*oled.setTextXY(1,0);
    oled.putString((String)read_sensors[SENSOR_LINE_L]);
    oled.putString(" : ");
    oled.putString((String)read_sensors[SENSOR_LINE_R]);
    oled.putString("      ");*/
    /*Serial.print(read_sensors[SENSOR_FL]);
    Serial.print(" | ");
    Serial.println(read_sensors[SENSOR_FR]);*/
    //si on detecte moins loin que la distance d'attaque
    if(read_sensors[SENSOR_FL] < detection_distance && abs(old_read[SENSOR_FL]-read_sensors[SENSOR_FL]) <= PARASIT_DIFF ) {
      next_state = ATTACK;
      //si on detecte moins loin que la distance de rush 
      
    } else {
      readLineSensors();
      if(read_sensors[SENSOR_LINE_L] > black_value && read_sensors[SENSOR_LINE_R] > black_value)
        next_state = BACK; /* TODO: faire un back en fonction du capteur */
      else
        next_state = SEARCH;
    }
    if(digitalRead(microstart)==LOW)
      next_state = STOP;
  }
}

void setup() {  
  Serial.begin(9600);
  Wire.begin();
  
  pinMode(microstart, INPUT);

  //init sensors
  setupSensors();

  oled.init();                      // Initialze SSD1306 OLED display
  oled.clearDisplay();              // Clear screen
  oled.setTextXY(0,0);              // Set cursor position, start of line 0
  oled.putString("Flash McQueen !");
  
  //init motors
  setupMotors();

  current_state = next_state = INIT;
  //MsTimer2::set(100, routine);
  //MsTimer2::start();
  oled.setTextXY(1,0);              
  oled.putString("READY !");
}

void loop() {
  routine();
    switch(current_state) {
      case INIT:
        oled.setTextXY(2,0);
        oled.putString("Waiting for");
        oled.setTextXY(3,0);
        oled.putString("microstart");
        old = read_sensors[SENSOR_L];
        next_state = START;
        break;
     
      case START:
        
        /*readAll();
        oled.setTextXY(4,0);
        if(abs(read_sensors[SENSOR_L]-old) > 2){
          rotate_sens = LEFT;
          oled.putString("Ennemi on left ");
        } else {
          rotate_sens = RIGHT;
          oled.putString("Ennemi on right");
        }*/
        if(digitalRead(microstart) == HIGH) {
          //MsTimer2::start();
          oled.setTextXY(0,0);
          oled.putString("Starting !");
          oled.setTextXY(3,0);
          oled.putString("Black : ");
          oled.putString((String)black_value);
          started = true;
          move(RIGHT, 200);
          delay(85);
          next_state = ATTACK;
        }
        break;
        
      case SEARCH:
        oled.setTextXY(2,0);
        oled.putString("Searching ...");
        desactiveBoost();
        move(rotate_sens, 100);
        //delay(100);
        //stop();
        break;
  
      //attack 
      case ATTACK:     
        oled.setTextXY(2,0);
        oled.putString("Attacking ...");
        desactiveBoost();
        move(FORWARD);
        delay(1000);
        break;
  
      //when we are at less than boost_distance toggle the boost
      case RUSH:
        oled.setTextXY(2,0);
        oled.putString("Rushing ...  ");
        move(FORWARD);
        if(count_time_boost < max_time_boost) {
          activeBoost();
          count_time_boost++;
        } else {
          desactiveBoost();
          count_time_boost = 0;
          next_state = BACK;
        }
        break;

      case FOLLOW_RIGHT:
        oled.setTextXY(2,0);
        oled.putString("Following R  ");
        move(FORWARD);
        setSpeedForEach(255, 200);
        break;

      case FOLLOW_LEFT: 
        oled.setTextXY(2,0);
        oled.putString("Following L  ");
        move(FORWARD);
        setSpeedForEach(200, 255);
        break;
        
      case BACK:
        oled.setTextXY(2,0);
        oled.putString("Back         ");
        move(BACKWARD);
        delay(400);
        stop();
        break;
        
      case STOP:
        started = 0;
        stop();
        oled.clearDisplay();
        oled.setTextXY(0,0);
        oled.putString("THE END !");
        break;
    }
  last_state = current_state;
  current_state = next_state;
  //delay(500);
}

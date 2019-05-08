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
uint8_t microstart = 7;

//constant
const int boost_distance = 15,
          detection_distance = 35,
          max_time_boost = 300;

int count_time_boost = 0;

boolean started = false;

//##### functions #####

//timed function 
void routine () {
  if(started) {
    readAll();
    oled.setTextXY(0,0);
    oled.putString((String)read_sensors[SENSOR_FL]);
    oled.putString("            ");
    oled.setTextXY(1,0);
    oled.putString((String)read_sensors[SENSOR_FR]);
    oled.putString("      ");
    /*Serial.print(read_sensors[SENSOR_FL]);
    Serial.print(" | ");
    Serial.println(read_sensors[SENSOR_FR]);*/
    //si on detecte moins loin que la distance d'attaque
    if(read_sensors[SENSOR_FR] < detection_distance  && read_sensors[SENSOR_FL] < detection_distance) {
      next_state = ATTACK;
      //si on detecte moins loin que la distance de rush
      if(read_sensors[SENSOR_FR] <= boost_distance  && read_sensors[SENSOR_FL] <= boost_distance) {
        next_state = RUSH;
      } else if (read_sensors[SENSOR_FR] < read_sensors[SENSOR_FL] && read_sensors[SENSOR_FL]-read_sensors[SENSOR_FR] > ERROR_DIFF) {
        next_state = FOLLOW_RIGHT;
      } else if (read_sensors[SENSOR_FR] > read_sensors[SENSOR_FL] && read_sensors[SENSOR_FR]-read_sensors[SENSOR_FL] > ERROR_DIFF) {
        next_state = FOLLOW_LEFT;
      } 
      
    } else {
      next_state = SEARCH;
    }
    /*if(digitalRead(microstart)==LOW)
      next_state = STOP;*/
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
        oled.putString("INIT state");
        next_state = START;
        break;
     
      case START:
        if(digitalRead(microstart) == LOW) {
          delay(3000);
          //MsTimer2::start();
          oled.setTextXY(0,0);
          oled.putString("Starting !");
          started = true;
          next_state = SEARCH;
        }
        break;
        
      case SEARCH:
        oled.setTextXY(2,0);
        oled.putString("Searching ...");
        desactiveBoost();
        move(RIGHT, 200);
        break;
  
      //attack 
      case ATTACK:     
        oled.setTextXY(2,0);
        oled.putString("Attacking ...");
        desactiveBoost();
        move(FORWARD);
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
        next_state = SEARCH;
        break;
        
      case STOP:
        oled.clearDisplay();
        oled.setTextXY(0,0);
        oled.putString("THE END !");
        break;
    }
  }
  last_state = current_state;
  current_state = next_state;
  //delay(1000);
}

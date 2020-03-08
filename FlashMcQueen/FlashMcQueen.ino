#include <ACROBOTIC_SSD1306.h>
#include "motors.h"
#include "sensors.h"

//#define DEBUG
//#define DEBUG_Detection

//state machine
#define INIT 1
#define TEST_CAPTEUR 11
#define DETECT_LIGNE_BLANCHE 12
#define START 2
#define SEARCH 3
#define ATTACK 4
#define FOLLOW_RIGHT 5
#define FOLLOW_LEFT 6
#define BACK 7
#define RUSH 8
#define STOP 9

//Pour l'affichage
int position_curseur = 2;
//

uint8_t current_state = 0, next_state = 0, last_state = 0;
//####end state machine def####

//pin for microstart
uint8_t microstart = 12;

//constant
const int boost_distance = 15,
          detection_distance = 30,
          max_time_boost = 100;

int count_time_boost = 0, old;

uint8_t rotate_sens = RIGHT;

boolean started = false;

int white_value = 300; //calibrate the white color

//##### functions #####
void init_affiche(){                      //initialise l'affichage et affiche le cadre
  oled.init();                            // Initialze SSD1306 OLED display
  oled.activateScroll();
  oled.clearDisplay();                    // Clear screen
  oled.setTextXY(0,0);                    // Set cursor position, start of line 0
  oled.putString("################");
  for(unsigned char i=1; i<=6; i++) { 
    oled.setTextXY(i,0);
    oled.putString("#              #");
  }
  oled.setTextXY(7,0);              
  oled.putString("################");
}


void affiche_S(const char* message,int ligne) { 
  oled.setTextXY(ligne,4);
  oled.putString(message);
}

void affiche_I(int message,int ligne) { 
  oled.setTextXY(ligne,1);
  oled.putString("   ");
  oled.setTextXY(ligne,1);
  oled.putNumber(message);
}
//timed function 
void routine() {
  if(started){
    //sei();
    //Serial.println("Int");
    old_read[SENSOR_FL] = read_sensors[SENSOR_FL];
    old_read[SENSOR_FR] = read_sensors[SENSOR_FR];
    readAll();
    
    /*oled.setTextXY(1,0);
    oled.putString((String)read_sensors[SENSOR_LINE_L]);
    oled.putString(" : ");
    oled.putString((String)read_sensors[SENSOR_LINE_R]);
    oled.putString("      ");*/
    /*Serial.print(read_sensors[SENSOR_FL]);
    Serial.print(" | ");
    Serial.println(read_sensors[SENSOR_FR]);*/
    //si on detecte moins loin que la distance d'attaque
    if(((abs(read_sensors[SENSOR_FR] - read_sensors[SENSOR_FL]) <= 2 && read_sensors[SENSOR_FL] < MAX_VALUE && read_sensors[SENSOR_FR] < MAX_VALUE)) || read_sensors[SENSOR_FR] < detection_distance  && read_sensors[SENSOR_FL] < detection_distance && abs(old_read[SENSOR_FL]-read_sensors[SENSOR_FL]) <= PARASIT_DIFF && abs(old_read[SENSOR_FR]-read_sensors[SENSOR_FR]) <= PARASIT_DIFF ) {
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

void stopBot() {
  next_state = STOP;
}

void setup() {  
  Serial.begin(9600);
  Wire.begin();
  
  pinMode(microstart, INPUT);

  //init sensors
  setupSensors();
  init_affiche();               //initialise l'affichage et affiche le cadre
  
  //init motors
  setupMotors();

  current_state = next_state = TEST_CAPTEUR; //INIT;
  oled.setTextXY(4,6);              
  oled.putString("READY");
  delay(1000);
  oled.clearDisplay();
}

void loop() {

    /*Serial.print(read_sensors[SENSOR_FL]);
    Serial.print(" | ");
    Serial.println(read_sensors[SENSOR_FR]);*/
    //routine();
  readAll();
    switch(current_state) {
      case TEST_CAPTEUR:
        #ifdef DEBUG
          oled.clearDisplay();
          oled.setTextXY(0,0);
          oled.putString("# TEST_CAPTEUR #");
          affiche_I(read_sensors[SENSOR_FL],2);
          affiche_S(" F Gauche ",2);
          affiche_I(read_sensors[SENSOR_FR],3);
          affiche_S(" F RIGHT ",3);
          affiche_I(read_sensors[SENSOR_L],4);
          affiche_S(" L ",4);
          affiche_I(read_sensors[SENSOR_LINE_L],5);
          affiche_S(" LB Gauche ",5);
          affiche_I(read_sensors[SENSOR_LINE_R],6);
          affiche_S(" LB RIGHT ",6);
          delay(500);
        #endif
        //if(read_sensors[SENSOR_L] == 3){
         // next_state = INIT;
          //next_state = TEST_CAPTEUR;
          next_state = DETECT_LIGNE_BLANCHE;

        //}
        break;

      case DETECT_LIGNE_BLANCHE:
        #ifdef DEBUG
          oled.clearDisplay();
          oled.setTextXY(0,0);
          oled.putString("#  DETECT_L_B  #");
          next_state = TEST_CAPTEUR;
        #endif
        #ifdef DEBUG_Detection
          oled.clearDisplay();
          oled.setTextXY(0,0);
          oled.putString("#  DETECT_L_B  #");
        #endif
          /*move(LEFT);
          delay(1000);
          move(FORWARD);
          delay(1000);
          move(RIGHT);
          delay(1000);
          move(BACKWARD);
          delay(1000);
          move(STOP);
          */

        if(read_sensors[SENSOR_LINE_L] > white_value){ 
          #ifdef DEBUG_Detection

            affiche_S(" LB Gauche ",1);
          #endif

          move(LEFT);
          delay(400);
          
        }else {
          #ifdef DEBUG_Detection
            affiche_S("           ",1);
          #endif
          move(FORWARD;255);
        }

        //move(STOP);
       //move(FORWARD);
       // next_state = TEST_CAPTEUR;
        break;

      case INIT:
        oled.setTextXY(0,0);
        oled.putString("#     INIT     #");

        delay(400);
        readAll();
        old = read_sensors[SENSOR_L];
        next_state = START;
        attachInterrupt(digitalPinToInterrupt(microstart), stopBot, FALLING);
        oled.clearDisplay();
        oled.setTextXY(0,0);
        oled.putString("#     START    #");
        break;
     
      case START:
       // readAll();
        
        
        if(abs(read_sensors[SENSOR_L]-old) > 2){
          rotate_sens = LEFT;
         // oled.clearDisplay();
          oled.setTextXY(4,13);
          oled.putString(" ");
          oled.setTextXY(4,3);
          oled.putString("|");
        } else {
          rotate_sens = RIGHT;
          oled.setTextXY(4,3);
          oled.putString(" ");
          oled.setTextXY(4,13);
          oled.putString("|");
          
        }
        
        if(digitalRead(microstart) == HIGH) {
          oled.clearDisplay();
          //oled.setTextXY(0,0);
          //oled.putString("Starting !");
          for(int i=5; i != 0; i--) {
            oled.setTextXY(3,7);
            oled.putNumber(i);
            delay(940);
          }
          started = true;
          move(rotate_sens);
          delay(210);
          move(FORWARD);
          activeBoost(); 
          next_state = ATTACK;
        }
        break;
        
      case SEARCH:
        /*oled.clearDisplay();
        oled.setTextXY(0,0);
        oled.putString("#    SEARCH   #");
        oled.setTextXY(2,0);
        oled.putNumber(read_sensors[SENSOR_FR]);
        */
        if((read_sensors[SENSOR_FR] < detection_distance && abs(read_sensors[SENSOR_FR] - old_read[SENSOR_FR]) < PARASIT_DIFF) || read_sensors[SENSOR_FL] < detection_distance){
          next_state = ATTACK;
          /*move(rotate_sens==LEFT ? RIGHT : LEFT);
          delay(50);*/
          //&& 
        } else{
          desactiveBoost();
          move(rotate_sens, 150);
        }
        break;
  
      //attack 
      case ATTACK:
        /*oled.setTextXY(0,0);
        oled.putString("#    ATTACK   #");
        */
        
        move(FORWARD);
        activeBoost();
        
        next_state = SEARCH;
        break;
  
      //when we are at less than boost_distance toggle the boost
      case RUSH:
        move(FORWARD);
        if(count_time_boost < max_time_boost) {
          activeBoost();
          count_time_boost++;
        } else {
          desactiveBoost();
          count_time_boost = 0;
          next_state = BACK;
        }
        /*oled.setTextXY(2,0);
        oled.putString("Rushing ...  ");*/
        break;

      case FOLLOW_RIGHT:
        /*oled.setTextXY(2,0);
        oled.putString("Following R  ");*/
        move(FORWARD);
        setSpeedForEach(255, 200);
        break;

      case FOLLOW_LEFT: 
        /*oled.setTextXY(2,0);
        oled.putString("Following L  ");*/
        move(FORWARD);
        setSpeedForEach(200, 255);
        break;
        
      case BACK:
        /*oled.setTextXY(2,0);
        oled.putString("Back         ");*/
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

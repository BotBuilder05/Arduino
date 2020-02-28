#include <ACROBOTIC_SSD1306.h>
#include "motors.h"
#include "sensors.h"


//state machine
#define INIT 1
#define TEST_CAPTEUR 11
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
          detection_distance = 35,
          max_time_boost = 100;

int count_time_boost = 0, old;

uint8_t rotate_sens = RIGHT;

boolean started = false;

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
  oled.setTextXY(ligne,5);
  oled.putString(message);
}

void affiche_I(int message,int ligne) { 
  oled.setTextXY(ligne,2);
  oled.putString("   ");
  oled.setTextXY(ligne,2);
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

void setup() {  
  Serial.begin(9600);
  Wire.begin();
  
  pinMode(microstart, INPUT);

  //init sensors
  setupSensors();
  init_affiche();               //initialise l'affichage et affiche le cadre
  
  //init motors
  setupMotors();

  /*timed interrupt*/
  /*cli();//stop interrupts
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 5000;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei();*/

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
    routine();
    switch(current_state) {
      case TEST_CAPTEUR:
        started = 1;
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
        next_state = TEST_CAPTEUR;
        delay(100);
        if(read_sensors[SENSOR_L] == 3)
          next_state = INIT;
        break;

      case INIT:
        oled.setTextXY(1,0);
        oled.putString("#     INIT     #");
        old = read_sensors[SENSOR_L]   ;
        if(abs(read_sensors[SENSOR_L]-old) > 2){
          rotate_sens = LEFT;
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
        delay(1000);
        next_state = START;
        break;
     
      case START:
       // readAll();
        oled.setTextXY(0,0);
        oled.putString("#     START    #");
        delay(1000);
        
        if(digitalRead(microstart) == HIGH) {
          oled.setTextXY(0,0);
          oled.putString("Starting !");
          oled.setTextXY(3,0);
          
          started = true;
          move(rotate_sens, 200);
          delay(200);
          move(FORWARD);
          activeBoost();
          delay(200);
          next_state = ATTACK;
        }
        break;
        
      case SEARCH:
        desactiveBoost();
        move(rotate_sens, 100);
        /*oled.setTextXY(2,0);
        oled.putString("Searching ...");*/
        break;
  
      //attack 
      case ATTACK:
        move(FORWARD);
        desactiveBoost();    
        /*oled.setTextXY(2,0);
        oled.putString("Attacking ...");*/
        
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

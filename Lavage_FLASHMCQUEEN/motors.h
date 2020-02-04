#define FORWARD 1
#define BACKWARD 2
#define LEFT 3
#define RIGHT 4

uint8_t motor_1_A = 3,
        motor_1_B = 4,
        motor_2_A = 5,
        motor_2_B = 6,
        motor_1_en = 9, //left motor
        motor_2_en = 10;  //right motor

uint8_t boost = 2;
boolean boosted = false;

void setupMotors (){
  for(int i=3; i<7; i++){
    pinMode(i, OUTPUT);
  }
  pinMode(motor_1_en, OUTPUT);
  pinMode(motor_2_en, OUTPUT);
  pinMode(boost, OUTPUT);
}

void toggleBoost(){
  boosted != boosted;
  digitalWrite(boost, boosted);
}

void activeBoost(){
  boosted = true;
  digitalWrite(boost, boosted);
}

void desactiveBoost(){
  boosted = false;
  digitalWrite(boost, boosted);
}

void stop() {
  digitalWrite(motor_1_en, LOW);
  digitalWrite(motor_2_en, LOW);
  PORTD = B00000000;
}

void setSpeed (int speed = 255) {
  analogWrite(motor_1_en, speed);
  analogWrite(motor_2_en, speed);
}

void setSpeedForEach(int left, int right) {
  analogWrite(motor_1_en, left);
  analogWrite(motor_2_en, right);
}

void move (uint8_t sens, int speed = 255){
  if(sens == FORWARD){
    PORTD = B01010000;
    /*digitalWrite(motor_1_A, LOW);
    digitalWrite(motor_1_B, HIGH);
    digitalWrite(motor_2_A, LOW);
    digitalWrite(motor_2_B, HIGH);*/
  } else if (sens == BACKWARD){
    PORTD = B00101000;
    /*digitalWrite(motor_1_A, HIGH);
    digitalWrite(motor_1_B, LOW);
    digitalWrite(motor_2_A, HIGH);
    digitalWrite(motor_2_B, LOW);*/   
  } else if(sens == LEFT) {
    PORTD = B01001000;
    /*digitalWrite(motor_1_A, HIGH);
    digitalWrite(motor_1_B, LOW);
    digitalWrite(motor_2_A, LOW);
    digitalWrite(motor_2_B, HIGH);*/
  } else if (sens == RIGHT){
    PORTD = B00110000;
    /*digitalWrite(motor_1_A, LOW);
    digitalWrite(motor_1_B, HIGH);
    digitalWrite(motor_2_A, HIGH);
    digitalWrite(motor_2_B, LOW);*/
  }
  setSpeed(speed);
}



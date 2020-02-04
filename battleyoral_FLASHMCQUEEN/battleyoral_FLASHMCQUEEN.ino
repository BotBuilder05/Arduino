#include "motors.h"

uint8_t microstart = 12, flag = 1;

void setup() {
  setupMotors();
}

void loop() {
 if(digitalRead(microstart)==HIGH){
  if(flag) {
      activeBoost();
      move(FORWARD);
      flag=0;
      delay(1000);
  }
  desactiveBoost();
  move(RIGHT);
 } else
  stop();
}

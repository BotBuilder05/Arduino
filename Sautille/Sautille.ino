#include <MsTimer2.h>
#include "Sautille.h"

/*   CONSTANTES   */

    uint8_t started = 0, white_line_detected = 0, detected = 0, too_near = 0, was_detected = 1;
    uint32_t counter = 0;

/*###          ###*/

void setup() {

    MsTimer2::set(100, interrupt);
    MsTimer2::start();

    Serial.begin(9600);
    pinMode(BUTTON ,INPUT_PULLUP); //le PIN GO est en mode entrée avec resistance de tirage au +5V
    pinMode(IR_AV_D,INPUT); //le PIN AV_D est en mode entrée avec resistance de tirage au 0V
    //pinMode(IR_AV_G,INPUT); //le PIN AV_G est en mode entrée avec resistance de tirage au 0V

    pinMode(leftWheel, OUTPUT); //le PIN leftWheel (10) est en mode sortie
    pinMode(rightWheel, OUTPUT); //le PIN rightWheel (11) est en mode sortie
    pinMode(leftWheelDirection0, OUTPUT); //le PIN leftWheelDirection0 (6) est en mode sortie
    pinMode(leftWheelDirection1, OUTPUT); //le PIN leftWheelDirection1 (7) est en mode sortie
    pinMode(rightWheelDirection0, OUTPUT); //le PIN rightWheelDirection0(8) est en mode sortie
    pinMode(rightWheelDirection1, OUTPUT); //le PIN rightWheelDirection1 (9) est en mode sortie

    pinMode(MAIN_IR, INPUT);

    last_state = next_state = current_state = INIT;
}

void loop() {

    switch(current_state) {
        case INIT:
            if(digitalRead(BUTTON) == LOW)
                next_state = START;
            break;

        case START:
            wait5();
            go(255);
            delay(250);
            //fuit
            //back(255);
            //delay(200);
            started = 1;
            next_state = SEARCH;

            //attaque
            /*next_state = ATTACK;
            turnRight(255, 255);
            delay(190);
            started = 1;
            go(255);
            delay(75);
            detected =1;*/

            break;

        case SEARCH:
            if(white_line_detected || too_near)
                next_state = ESCAPE;
            else if(detected == 0) {
                if(was_detected)
                    turnRight(255, 255);
                else
                    turnLeft(255, 255);
            } else
                next_state = ATTACK;
            break;

        case ATTACK:
            if(white_line_detected || too_near)
                next_state = ESCAPE;
            else if(detected){
                if(last_state == ATTACK){
                    counter++;
                    if(counter == 45){
                        counter = 0;
                        back(255);
                        delay(100);
                    }
                } else
                    counter = 0;
                go(255);
                delay(100);
            } else {
                next_state = SEARCH;
                if(was_detected)
                    was_detected = 0;
                else
                    was_detected = 1;
            }
            break;

        case ESCAPE:
            back(255);
            delay(600);
            if(too_near) {
                go(255);
                next_state = ATTACK;
                break;
            } else
                turnRight(255, 255);

            delay(100);
            next_state = SEARCH;
            break;
    }

    last_state = current_state;
    current_state = next_state;
}

void wait5(){
    for(int i=0; i<2; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(1000);
        digitalWrite(LED_BUILTIN, LOW);
        delay(1000);
    }
    digitalWrite(LED_BUILTIN, HIGH);
    delay(800);
    return;
}

void interrupt(){
    if(started) {
        int ir = analogRead(MAIN_IR);
        if(analogRead(IR_AV_D) > DETECT_WHITE_LINE_VAL){
            white_line_detected = 1;
            detected = 0;
            too_near = 0;
        } else if(ir > DETECT_MIN) {
            white_line_detected = 0;
            detected = 1;
            too_near = 0;
        } else if (ir > DETECT_MAX) {
            white_line_detected = 0;
            detected = 0;
            too_near = 1;
        } else {
            white_line_detected = 0;
            detected = 0;
            too_near = 0;
        }
    }
}

void go(uint8_t wheelSpeed){
  digitalWrite(leftWheelDirection0, LOW);
  digitalWrite(leftWheelDirection1, HIGH);
  digitalWrite(rightWheelDirection0, LOW);
  digitalWrite(rightWheelDirection1, HIGH);
  analogWrite(leftWheel, wheelSpeed);
  analogWrite(rightWheel, wheelSpeed);
}
/* FIN AVANCE */

/* RECULE */
void back(uint8_t wheelSpeed){
// mouvement
    digitalWrite(leftWheelDirection0, HIGH);
    digitalWrite(leftWheelDirection1, LOW);
    digitalWrite(rightWheelDirection0, HIGH);
    digitalWrite(rightWheelDirection1, LOW);
    analogWrite(leftWheel, wheelSpeed);
    analogWrite(rightWheel, wheelSpeed);
}
/* FIN RECULE */

/* TOURNE A GAUCHE */
void turnRight(uint8_t leftWheelSpeed, uint8_t rightWheelSpeed){
// mouvement
    digitalWrite(leftWheelDirection0, LOW);
    digitalWrite(leftWheelDirection1, HIGH);
    digitalWrite(rightWheelDirection0, HIGH);
    digitalWrite(rightWheelDirection1, LOW);
    analogWrite(leftWheel, leftWheelSpeed);
    analogWrite(rightWheel, rightWheelSpeed);
}
/* FIN TOURNE A GAUCHE */

/* TOURNE A DROITE */
void turnLeft(uint8_t leftWheelSpeed, uint8_t rightWheelSpeed){
// mouvement
    digitalWrite(leftWheelDirection0, HIGH);
    digitalWrite(leftWheelDirection1, LOW);
    digitalWrite(rightWheelDirection0, LOW);
    digitalWrite(rightWheelDirection1, HIGH);
    analogWrite(leftWheel, leftWheelSpeed);
    analogWrite(rightWheel, rightWheelSpeed);
}
/* FIN TOURNE A DROITE */

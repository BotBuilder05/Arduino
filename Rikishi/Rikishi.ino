#include <Arduino.h>
#include <MsTimer2.h>
#include "Rikishi.h"

/*  Fonction appelé par les interruptions
 *  de la lib Servo
 *  Return : void
 */

int counter = 0; //compteur d'interruption

/* Fonction qui attend 5 seconde
 * avec blink de la led
 * return: void
 */
void wait5()
{
    for(int i=0; i<2; i++) {
        digitalWrite(ledPin, HIGH);
        delay(1000);
        digitalWrite(ledPin, LOW);
        delay(1000);
    }
    digitalWrite(ledPin, HIGH);
    delay(1000);
    return;
}

/* Fonction qui fait avancer
 * le robot a une vitesse donnée
 * en % et un sens
 * return: void
 */
void move_(uint8_t speed_per_cent, uint8_t sens)
{
    int speed = speed_per_cent * 255 / 100;

    //Active les 2 moteurs
    analogWrite(mot_1_activ, speed);
    analogWrite(mot_2_activ, speed);

    digitalWrite(mot_1_droit, !sens);
    digitalWrite(mot_1_gauche, sens);

    digitalWrite(mot_2_droit, !sens);
    digitalWrite(mot_2_gauche, sens);
}

/* Fonction qui fait tourner le robot
 * dans un sens et avec une vitesse
 * return: void
 */
void tourne(uint8_t speed_per_cent, uint8_t sens) {

    int speed = speed_per_cent * 255 / 100;

    //Active les 2 moteurs
    analogWrite(mot_1_activ, speed);
    analogWrite(mot_2_activ, speed);

    digitalWrite(mot_1_droit, !sens);
    digitalWrite(mot_1_gauche, sens);

    digitalWrite(mot_2_droit, sens);
    digitalWrite(mot_2_gauche, !sens);
}

/* Fonction qui stoppe le robot
 * return: void
 */
void stop(){
    move_(0, RECULE);
}

/* Fonction qui lit les
 * 3 capteurs (fin plateau, pelle, et ultrason)
 * return: void
 */
void read_capts(){
    if(digitalRead(END_LIM) == HIGH) {
        detected = 0;
        end_detected = 1;
    } else if (analogRead(NEAR_CAPT) > 300) {
        detected = 1;
        end_detected = 0;
    } else {
        digitalWrite(SEND_PIN, LOW);
        read_echo = pulseIn(READ_PIN, HIGH);
        if(read_echo / 58 < detect_min) {
            detected = 1;
            end_detected = 0;
        }
        digitalWrite(SEND_PIN, HIGH);
    }
}

/*setup des pins */
void setup() {

    Serial.begin(9600);

    pinMode(BUTTON, INPUT_PULLUP);
    pinMode(NEAR_CAPT, INPUT);
    pinMode(READ_PIN, INPUT);
    pinMode(END_LIM, INPUT);

    pinMode(ledPin, OUTPUT);
    pinMode(mot_1_activ, OUTPUT);
    pinMode(mot_1_droit, OUTPUT);
    pinMode(mot_1_gauche, OUTPUT);
    pinMode(mot_2_activ, OUTPUT);
    pinMode(mot_2_droit, OUTPUT);
    pinMode(mot_2_gauche, OUTPUT);

    pinMode(SEND_PIN, OUTPUT);

    MsTimer2::set(1000, read_capts);
    MsTimer2::start();

    digitalWrite(SEND_PIN, HIGH);
}

/* boucle sur une machine d'état */
void loop() {

    switch(current_state) {
        case INIT:
            if(digitalRead(BUTTON) == LOW)
                next_state = START;
            break;

        case START:
            wait5();
            next_state = SEARCH;
            break;

        case SEARCH:
            if(end_detected)
                end_detected = END_OF_LIMIT;
            else if(detected)
                next_state = ATTACK;
            else {
               tourne(100, RIGHT);
               digitalWrite(ledPin, LOW);
            }
            break;

        case ATTACK:
            if(detected && !end_detected) {
                move_(100, AVANCE);
                digitalWrite(ledPin, HIGH);
            }
            else
                next_state = SEARCH;
            break;

        case END_OF_LIMIT:
            if(end_detected)
                move_(100, RECULE);
            else
                next_state = SEARCH;
            break;
    }

    last_state = current_state;
    current_state = next_state;
}

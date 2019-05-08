#include <Arduino.h>
#include <MsTimer2.h>
#include "Rikishi.h"

int move_speed = 100, rotate_speed = 100, started = 0, was_detected = 0;

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
    delay(500);
    return;
}

/* Fonction qui fait avancer
 * le robot a une vitesse donnée
 * en % et un sens
 * return: void
 */
void move_(int speed_per_cent, uint8_t sens)
{
    int speed = map(speed_per_cent, 0, 100, 0, 255);

    //Active les 2 moteurs
    analogWrite(mot_1_activ, speed);
    analogWrite(mot_2_activ, speed);

    digitalWrite(mot_1_droit, sens);
    digitalWrite(mot_1_gauche, !sens);

    digitalWrite(mot_2_droit, sens);
    digitalWrite(mot_2_gauche, !sens);
}

/* Fonction qui fait tourner le robot
 * dans un sens et avec une vitesse
 * return: void
 */
void tourne(int speed_per_cent, uint8_t sens) {

    int speed = map(speed_per_cent, 0, 100, 0, 255);

    if(sens == LEFT) {
        digitalWrite(mot_1_droit, 1);
        digitalWrite(mot_1_gauche, 0);

        digitalWrite(mot_2_droit, 0);
        digitalWrite(mot_2_gauche, 1);
    } else {
        digitalWrite(mot_1_droit, 0);
        digitalWrite(mot_1_gauche, 1);

        digitalWrite(mot_2_droit, 1);
        digitalWrite(mot_2_gauche, 0);
    }

    //Active les 2 moteurs
    analogWrite(mot_1_activ, speed);
    analogWrite(mot_2_activ, speed);
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
    if(started) {
        if(digitalRead(END_LIM) == HIGH) {
            detected = 0;
            end_detected = 1;
        } else if (analogRead(NEAR_CAPT) < 1000) {
            detected = 1;
            end_detected = 0;
        } else {
            digitalWrite(SEND_PIN, LOW);
            read_echo = pulseIn(READ_PIN, HIGH);
            if(read_echo / 58 < detect_min) {
                detected = 1;
                Serial.println(read_echo / 58);
            } else
                detected = 0;
            digitalWrite(SEND_PIN, HIGH);
            end_detected = 0;
        }
    }
}

/*setup des pins */
void setup() {

    MsTimer2::set(250, read_capts);
    MsTimer2::start();

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
            tourne(rotate_speed, RIGHT);
            delay(600);
            move_(move_speed, AVANCE);
            delay(1000);
            started=1;
            next_state = SEARCH;
            break;

        case SEARCH:
            if(end_detected)
                next_state = END_OF_LIMIT;
            else if(detected)
                next_state = ATTACK;
            else {
                if(was_detected) {
                    tourne(rotate_speed, LEFT);
                    delay(100);
                } else {
                    tourne(rotate_speed, RIGHT);
                    delay(100);
                }
                digitalWrite(ledPin, LOW);
            }
            break;

        case ATTACK:
            if(detected) {
                move_(move_speed, AVANCE);
                digitalWrite(ledPin, HIGH);
                delay(500);
            } else if (end_detected)
                next_state = END_OF_LIMIT;
            else {
                next_state = SEARCH;
                if(was_detected)
                    was_detected = 0;
                else
                    was_detected = 1;
            }
            break;

        case END_OF_LIMIT:
            if(end_detected) {
                move_(move_speed, RECULE);
                delay(1000);
            }
            else
                next_state = SEARCH;
            break;
    }

    last_state = current_state;
    current_state = next_state;
}

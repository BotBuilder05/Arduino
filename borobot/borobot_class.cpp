#include <Arduino.h>
#include <Servo.h>
#include "borobot.h"

//######## implï¿½mentation des fonctions ########
Borobot::Borobot() : // dï¿½finitions des pins
    mode(MOD_INIT),
    led1(LED_1), led2(LED_2),
    but1(BUT_1), but2(BUT_2),
    capt_white_line1(A1), capt_white_line2(A0), capt_ir1(A2), capt_ir2(A3),
    moteur_droite_avant(12), moteur_droite_arriere(11), moteur_gauche_avant(10), moteur_gauche_arriere(9)
{
    volet.attach(4);
    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);
    pinMode(but1, INPUT_PULLUP);
    pinMode(but2, INPUT_PULLUP);
    pinMode(capt_white_line1, INPUT);
    pinMode(capt_white_line2, INPUT);
    pinMode(capt_ir1, INPUT);
    pinMode(capt_ir2, INPUT);
    pinMode(moteur_droite_arriere, OUTPUT);
    pinMode(moteur_droite_avant, OUTPUT);
    pinMode(moteur_gauche_arriere, OUTPUT);
    pinMode(moteur_gauche_avant, OUTPUT);
}

void Borobot::blink_led_with_temp(int repetion, int time, uint8_t led_to_blink){
    uint8_t *led;
    if(led_to_blink == 1)
        led = &led1;
    else
        led = &led2;
    uint8_t val = HIGH;
    for(int i=0; i < repetion; i++){
        digitalWrite(*led, val);
        delay(time);
        if (val == HIGH)
          val = LOW;
        else
          val = HIGH;
    }
}

void Borobot::stop(){
    digitalWrite(moteur_droite_arriere, LOW);
    digitalWrite(moteur_droite_avant, LOW);
    digitalWrite(moteur_gauche_arriere, LOW);
    digitalWrite(moteur_gauche_avant, LOW);
}

void Borobot::avance(){
    digitalWrite(moteur_droite_avant, HIGH);
    digitalWrite(moteur_gauche_avant, HIGH);
}

void Borobot::recule(){
    digitalWrite(moteur_droite_arriere, HIGH);
    digitalWrite(moteur_gauche_arriere, HIGH);
}

void Borobot::tourne(uint8_t sens){
    this->stop();
    if(sens == ROT_DROITE){
        digitalWrite(moteur_droite_arriere, HIGH);
        digitalWrite(moteur_gauche_avant, HIGH);
    } else if (sens == ROT_GAUCHE) {
        digitalWrite(moteur_gauche_arriere, HIGH);
        digitalWrite(moteur_gauche_avant, HIGH);
    }
}

void Borobot::waitForClick(uint8_t but){
    while(digitalRead(but) != HIGH);
}
 void Borobot::move_volet(uint8_t pos){
    volet.write(pos);
 }

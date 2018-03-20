#include <Arduino.h>
#include "borobot.h"
//Initilisation des pins du robot
Borobot bot;
int x(0);

void Robot50HzInterrupt() {
    x++;
    if(x%10 == 0)
        Serial.println(x);
    Serial.println("In interrupt");
}

void setup() {
    Serial.begin(9600);
    bot.move_volet(VOLET_CLOSE);
}

void loop() {
    Serial.println("ok");
    bot.waitForClick(BUT_1);
    bot.blink_led_with_temp(5, 1000, LED_1);
    bot.move_volet(VOLET_OPEN);
    bot.avance();
    delay(5000);
    bot.stop();
    bot.tourne(ROT_DROITE);
}


#ifndef BOROBOT_H_INCLUDED
#define BOROBOT_H_INCLUDED

#include <Arduino.h>
#include <Servo.h>

//définition des modes du robot
#define MOD_INIT 1
#define MOD_WAIT 2
#define MOD_SEARCH 3
#define MOD_ATCK_1 4

//definition des sens de rotation
#define ROT_DROITE 1
#define ROT_GAUCHE 0

class Borobot {

    public:
        Borobot(); //constructeur
        void blink_led_with_temp(int, int, uint8_t);
        void avance();
        void recule();
        void tourne(uint8_t); //sens = 1 = sens horaire / sens = 0 = sens antihoraire
        void stop();
        void waitForClick(uint8_t);

    private:
        uint8_t led1, led2;
        uint8_t but1, but2;
        uint8_t capt_white_line1, capt_white_line2, capt_ir1, capt_ir2;
        uint8_t moteur_droite_avant, moteur_droite_arriere, moteur_gauche_avant, moteur_gauche_arriere;
        Servo serv;
        uint8_t mode;
};

#endif // BOROBOT_H_INCLUDED


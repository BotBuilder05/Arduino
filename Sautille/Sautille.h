#ifndef SAUTILLE_H_INCLUDED
#define SAUTILLE_H_INCLUDED

//##machine d'état##//
#define INIT 1
#define START 2
#define ESCAPE 3
#define ATTACK 4
#define SEARCH 5

uint8_t last_state, current_state, next_state;

#define BUTTON 7
#define IR_AV_D A0
#define IR_AV_G A1
#define MAIN_IR A3

#define leftWheel 11 //PIN D10 est le PWM de la roue GAUCHE
#define rightWheel 10 //PIN D11 est le PWM de la roue DROITE
#define leftWheelDirection0 12 //PIN D6 est la direction 0 de la roue GAUCHE
#define leftWheelDirection1 13 //PIN D7 est la direction 1 de la roue GAUCHE
#define rightWheelDirection0 8 //PIN D8 est la direction 0 de la roue DROITE
#define rightWheelDirection1 9

#define DETECT_WHITE_LINE_VAL 600
#define DETECT_MIN 150
#define DETECT_MAX 650


//PROLOGUE
void go(uint8_t);
void turnRight(uint8_t, uint8_t);
void turnLeft(uint8_t, uint8_t);
void back(uint8_t);
void interrupt(void);

#endif // SAUTILLE_H_INCLUDED

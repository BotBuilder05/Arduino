#ifndef RIKISHI_H_INCLUDED
#define RIKISHI_H_INCLUDED

#include <Arduino.h>

/* défintion pour la machine d'état*/
uint8_t current_state, last_state, next_state;
#define INIT 0
#define START 1
#define SEARCH 2
#define ATTACK 3
#define END_OF_LIMIT 4
#define END 5

/* définition
 * des pins
 */

// Capteur ir contact = A1 pour desactiver la fonction finplateau
#define SEND_PIN 13
#define READ_PIN 12
long read_echo;
const uint8_t detect_min = 35; // distance de detection en cm

//capteur sur la pelle
#define NEAR_CAPT A1

#define ledPin 3  /* anode led verte démarrage D3*/
#define mot_1_activ 11  /* Moteur 1 Droit*/
#define mot_2_activ 6 /* Moteur 2*/
#define mot_1_gauche 10 /* Moteur 1*/
#define mot_1_droit 9  /* Moteur 1*/
#define mot_2_gauche 4 /* Moteur 2/Gauche*/
#define mot_2_droit 7 /* Moteur 2/Gauche*/

#define BUTTON 2  /* commute marche arriere*/

#define END_LIM 8//detecteur fin plateau

// sens de rotation
#define RIGHT 1
#define LEFT 2
//sens de direction
#define AVANCE 0
#define RECULE 1

//variable global qui indique si détecté et si on est au bout du plateau
uint8_t detected = 0;
uint8_t end_detected = 0;

/* prologue des fonctions */
void wait5();
void avance(uint8_t);
void tourne(uint8_t, uint8_t);
void stop();
void read_capts();

#endif // RIKISHI_H_INCLUDED

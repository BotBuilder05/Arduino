/*
Ce programme utilise une interruption
La fonction detecter est appellee a chaque interruption
Si la valeur est superieur au seuil alors la led
est allumée.
La fonction loop se charge d'eteindre la led.
Attetion compte tenu de la vitesse de l'interruption la led clignote tres
vite
*/
#define FILTRE_IR 20

int beDetect;
const int capteurIR = A2;   //capteur IR

volatile int valeurLue=0; //debug

volatile int detection;

//atomic print
#include <util/atomic.h>


const int ledPin = 13;          //PIN de la led à changer
int  tab_valeur_lue[20]; //debug
int moyenne;

int detecter() {
    
     int valeurLueBrut1; //debug
     int valeurLueBrut2; //debug
     int valeurAcc1; //debug
     static int valeurLueBrut[20];
     static int valeurAcc[20];
     int bg;
     int somme;
     int seuil = 90;

     // Lecture capteur
     valeurLueBrut1 = analogRead(capteurIR);
     valeurLueBrut[0] = valeurLueBrut1;
          
     
     for(int bg = 0 ; bg < FILTRE_IR ; bg++){
       valeurLueBrut[FILTRE_IR-bg] = valeurLueBrut[FILTRE_IR-(bg+1)] ; 
     }
     

     for(bg = 0 ; bg < FILTRE_IR ; bg++){
       somme +=  valeurLueBrut[FILTRE_IR];
     }
     /*
     90 -> 30cm
     137 -> 20cm
     250 -> 10cm
     450 -> 5cm
     */
     moyenne = somme/FILTRE_IR;
     
     if (moyenne > seuil) {
       detection = 1;
       digitalWrite(ledPin, HIGH);
     }
    return detection; 
     memcpy(tab_valeur_lue,valeurLueBrut,sizeof(valeurLueBrut));
}

void setup() {

  int CalibrationIR;
  Serial.begin(115200);

  digitalWrite(ledPin, LOW);

  attachInterrupt(0, detecter, LOW);

}

void loop() {
     ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
       Serial.print("Debug value brut : ");
//       Serial.println(valeurLueBrut1);


       Serial.print("Moyenne : ");
//       Serial.println(moyenne);

       Serial.print("Detection: ");
       Serial.println(detection);
       detection=0;
       digitalWrite(ledPin, LOW);
       Serial.print("Detection reset: ");
       Serial.println(detection);
     }

     //delay(250);
     digitalWrite(ledPin, LOW);
}

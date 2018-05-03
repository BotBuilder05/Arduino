/*
Ce programme utilise l'interruption timer 
de la librairie Servo.cpp(modifié)
La fonction detecter est appellee a chaque interruption
Si la valeur est superieur au seuil alors la led
est allumée.
La fonction loop se charge d'eteindre la led.
Attetion compte tenu de la vitesse de l'interruption la led clignote tres
vite
*/
#define POSITION_POSEE 20
#define FILTRE_IR 20

int beDetect;
const int capteurIR = A2;   //capteur IR

int valeurLue; //debug
const int servoPin = 9;     //PIN de la led à changer


int valeurLueBrut1; //debug
int valeurLueBrut2; //debug
int valeurAcc1; //debug

int detection;

// Import biblio gestion servo-moteur
#include <Servo.h>

//atomic print
#include <util/atomic.h>

//creation de l'objet servo
Servo servo;

const int ledPin = 13;          //PIN de la led à changer
int  tab_valeur_lue[20]; //debug
int moyenne;

int detecter() {
    
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
     }
    return detection; 
     memcpy(tab_valeur_lue,valeurLueBrut,sizeof(valeurLueBrut));
}

void monter() {
    /*
    En bas roues relevées, position servo 20°
    servo.write(POSITION_POSEE); //Necessaire ?
    Serial.println("Monter - Roue en l'air - Plateau en bas ");
    */
    servo.write(POSITION_POSEE);
}
      int cpt=0;

void Robot50HzInterrupt() {   
    //int valeurLue=5; //debug
    
    if (cpt==5){
      beDetect = detecter();
      cpt=0;
    }
    cpt+=1;
    
    if(beDetect){
        digitalWrite(ledPin, HIGH);
    }
}

void setup() {

  int CalibrationIR;
  Serial.begin(9600);

  servo.attach(servoPin);
  digitalWrite(ledPin, LOW);

}

void loop() {
     ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
       Serial.print("Debug value brut : ");
       Serial.println(valeurLueBrut1);


       Serial.print("Moyenne : ");
       Serial.println(moyenne);

       Serial.print("Detection: ");
       Serial.println(detection);
       detection=0;
       digitalWrite(ledPin, LOW);
       Serial.print("Detection: ");
       Serial.println(detection);
     }

     //delay(250);
     digitalWrite(ledPin, LOW);
     monter();
}

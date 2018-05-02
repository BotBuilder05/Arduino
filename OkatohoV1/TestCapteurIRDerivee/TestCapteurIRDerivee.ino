const int capteurIR = A2;   //capteur IR1 Ligne blanche 
int valeurLue; //debug
int detection;
int status_detection; //flag de detection

// Import biblio gestion servo-moteur
#include <Servo.h>
//creation de l'objet servo
Servo servo;

const int servoPin = 9;          //PIN de la led à changer
const int ledPin = 4;          //PIN de la led à changer
int beDetect;



#define POSITION_POSEE 20
#define POSITION_HAUTE 47 

int valeurLueBrut1; //debug
     int moyen1, moyen2;


void setup() {
   // initialize serial communication at 115200 bits per second:
   Serial.begin(115200);
   // put your setup code here, to run once:
   detection = 0;
   status_detection=0; //flag de detection

    // attaches the servo on pin to the servo object
    servo.attach(servoPin);
}

int test;  //debug


#define FILTRE_IR 20

int detecter() {
  static int valeurLueBrut[20];
  static int valeurAcc[20];
    // lecture capteur frontal
    // Detect de 30 a 800 - valeur capteur (à confirmer)

    //variable pour stocker la valeur lue après conversion
    test++;

    valeurLueBrut1 = analogRead(capteurIR);
    valeurLueBrut[0] = valeurLueBrut1;
    valeurAcc[0] = abs(valeurLueBrut[0] - valeurLueBrut[1]);
      
    //valeurAcc =0;
    for(int bg = 0 ; bg < FILTRE_IR ; bg++){
      valeurLueBrut[FILTRE_IR-bg] = valeurLueBrut[FILTRE_IR-(bg+1)] ;    
      valeurAcc[FILTRE_IR-bg] = valeurAcc[FILTRE_IR-(bg+1)];
      }
      
     for(int bm = 0 ; bm < 9 ; bm++){
        moyen1=valeurAcc[bm]+valeurAcc[bm+1];
     }
     moyen1=moyen1/10;
     
     for(int bm = 10 ; bm < 19 ; bm++){
        moyen2=valeurAcc[bm]+valeurAcc[bm+1];
     }
     moyen2=moyen2/10;

     valeurLue=abs(moyen1-moyen2);
  
/*
    // Pour Valeur brut  
    valeurLue = valeurLueBrut1;
*/
 
//     for(int bg = 0 ; bg < FILTRE_IR ; bg++){
//      valeurLueBrut[FILTRE_IR-bg] = valeurLueBrut[FILTRE_IR-(bg+1)] ;
//      
//      }

    //valeurLue = analogRead(capteurIR);
  
    // DEBUG - Affichage valeur capteur
    //Serial.print("DETECTER - valeurLue : ");
    //Serial.println(valeurLue);
    
    //on saute une ligne entre deux affichages
    //Serial.println();
    //on attend une demi-seconde pour que l'affichage ne soit pas trop rapide
    //delay(100);

    // Si valeur superieur a 30 on detecte quelque chose 
    //if (valeurLue>130){
     //if (valeurLue>170){
     int seuil = 10;
     //Pour valeur brut
     //int seuil = 200;
     if (valeurLue > seuil) {
        detection = 1;
     }
    status_detection=1;
    return detection;
/*
    bord_circuit_analog = analogRead(capteurIR0);
    bord_circuit_analog += analogRead(capteurIR1);
    if(bord_circuit_analog > 300) {
        bord_circuit = 1;
      }
    else {
        bord_circuit = 0;
    }
    */
}

void Robot50HzInterrupt() {
    beDetect = detecter();
    if(beDetect){
        digitalWrite(ledPin, HIGH);
    }
}
void loop() {

}

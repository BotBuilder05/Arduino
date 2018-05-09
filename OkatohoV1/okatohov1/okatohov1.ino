/*
# Strategie
* Mise sous tension -> initialisation servo moteur (pied en particulier pour avoir la position de depart)
* Appuie sur bouton cote adversaire (bouton droit ou bouton gauche)
 Attendre 5 secondes
* Demarrage jupe
* Avance
  - si dojo Tourne de 10°
* Detection
  - si detection avance
  - si non Tourne 10°
*/
#define DEBUG
#define DEBUG_Detection
//#define SLOW 

//Variable Machine d'etat
#define DEPART 1
#define ATT_5_SEC 2
#define JUPE 3
#define ATTAQUE_AVEUGLE 4
#define CHERCHE_ADV 5
#define ATTAQUE 6

// Pin definition
const int buttonOeilGStartPin=14;
const int buttonChapeauDStartPin=12;
const int moteurJupe5V = 5;
const int moteurJupeGND = 6;

const int ledOeilPin=11;
const int ledChapeauPin=4;
const int ledCorp1Pin=7;
const int ledCorp2Pin=10;

int sState , sStateNext;
int detect=0;
//variable pour les boutons
int buttonPressed;
//variable pour la jupe
 int nbPas=0;
//variable pour les LED
#define ledOn 0
#define ledOff 1

//variable pour les servos et la fonction avance
#define SERVO_D 3		//pin du servoD
#define SERVO_M 9		//pin du servoM

//#include <stdlib.h> 
#include <Servo.h>
Servo servoD;  // creation d'un objet servo pour controler un servo
Servo servoM;

const int DIR_AVANT = 0;   // direction souhaité
const int DIR_GAUCHE = 1;
const int DIR_ARRIERE = 2;
const int DIR_DROITE = 3;
int dirActuelPied = DIR_AVANT;
int dirSouhaitePied = DIR_AVANT;

int tabDirPied[2][4]={
  {50,95,140,5},
  {140,5,50,95}
};

const int HAUTE_AVANT = 0;
const int HAUTE_ARRIERE = 1;
boolean posActuelPied = HAUTE_AVANT ; // memorise la position HAUTE_AVANT ou HAUTE_ARRIERE du pied

int tabPosPied[2] = {10,170};
int posCentrePied = 90;
int tempoServoD = 500;
int tempoServoM = 700;

// Capteurs
const int capteurIRavant = A6;
const int capteurIRdroit = A3;
const int capteurIRarriere = A1;
const int capteurIRgauche = A2;

int capteurList[4] = {capteurIRavant, capteurIRgauche, capteurIRarriere, capteurIRdroit};
int maxarray=sizeof(capteurList)/sizeof(*capteurList);
int capteurQuiDetect;

void setup() {
	#ifdef DEBUG
		Serial.begin(9600 );
	#endif
	// On initialise les leds
	pinMode(ledOeilPin, OUTPUT);
	pinMode(ledChapeauPin, OUTPUT);
	pinMode(ledCorp1Pin, OUTPUT);
	pinMode(ledCorp2Pin, OUTPUT);
	digitalWrite(ledOeilPin, ledOff);
	digitalWrite(ledChapeauPin, ledOff);
	digitalWrite(ledCorp1Pin, ledOff);
	digitalWrite(ledCorp2Pin, ledOff);
	// On initialise la pin du bouton
	pinMode(buttonOeilGStartPin, INPUT_PULLUP);
	digitalWrite(buttonOeilGStartPin,HIGH );
	pinMode(buttonChapeauDStartPin, INPUT);
	// On initialise les servos
	servoM.attach(SERVO_M,850,2220);  // attaches l'objet servo a la pin 9
 	servoD.attach(SERVO_D,700,2700);
 	servoM.write(tabPosPied[posActuelPied]);
	servoD.write(tabDirPied[posActuelPied][dirActuelPied]);
	// On initialise les capteurs
        pinMode(capteurIRavant, INPUT);
        pinMode(capteurIRdroit, INPUT);
        pinMode(capteurIRarriere, INPUT);
        pinMode(capteurIRgauche, INPUT);
  // On initialise la jupe
        pinMode(moteurJupe5V, OUTPUT);
        pinMode(moteurJupeGND, OUTPUT);
        digitalWrite(moteurJupe5V, 0);
        digitalWrite(moteurJupeGND, 0);
        
	// On initialise la machine d'etat 
	sState=DEPART;
}



int lectureCapteur(int capteur) {
        int valeurLue=0;

        valeurLue = analogRead(capteur);
        if(valeurLue>900){
             valeurLue = analogRead(capteur);
        }
			#ifdef DEBUG_Detection
        		Serial.print(capteur);
        		Serial.print(" : ");
        		Serial.println(valeurLue);
        	#endif
        delay(15); // En theorie on peut encore descendrejusqu'a 5ms vu que chaque capteur est lisible toutes les 20ms(datasheet)
	return valeurLue;
}


int detection() {
	int detectStatus;
	int seuil=340;
	int valueCapteur[4];
	/* This finction should
		* return 0 if it detects 
		* return 1 othewise
	"This should be called by Timer interruption" pas sure!

 	- lit 1 fois chaque capteurs et on boucle 2 fois -> TABLEAUX
	- Detection si les 2 valeurs pour un meme capteurs sont superieures au seuil
	- Renvoir si detection ou non (boolean)
	- Affecte la variable de direction si detection
	*/
	// retourner le capteur qui a detecter  via une variable global ou iun pointeur
    for (int i=0;i<maxarray;i++) {
        valueCapteur[i]=lectureCapteur(capteurList[i]);
    }
	for (int i=0; i<maxarray;i++) {
		if (valueCapteur[i] > seuil) {  // On assume pour le moment qu'une seule valeur peut etre superieure au seuil
			capteurQuiDetect=i;
			#ifdef DEBUG_Detection
				Serial.print("capteurQuiDetect : ");
				Serial.print(capteurQuiDetect);
				Serial.print("  ");
				Serial.println(capteurList[capteurQuiDetect]);
			#endif
			detectStatus = 1;
			break;
		}
		else{
			Serial.println("rien ");
			detectStatus = 0;
		}
	}
	return detectStatus;
}

void Robot50HzInterrupt() {
	Serial.print("  ");
}


int identifyButtonPress() {
	// 0 -> bouton droite
	// 1 -> bouton gauche // oeil -> led oeil 
	// Tant que vrai, lecture bouton 1 lecture bouton 2
	int buttonOeilGStartState=1;
	int buttonChapeauDStartState=1;

	while ((buttonOeilGStartState == 1) && (buttonChapeauDStartState == 1)) {
		buttonOeilGStartState = digitalRead(buttonOeilGStartPin);
		buttonChapeauDStartState = digitalRead(buttonChapeauDStartPin);
    		//filtrée ici le rebond...
    		#ifdef DEBUG
      			Serial.print("START - Etat bouton oeil: ");
      			Serial.println(buttonOeilGStartState);
      			Serial.print("START - Etat bouton chapeau: ");
      			Serial.println(buttonChapeauDStartState);
    		#endif
	}
	if (buttonOeilGStartState == 0){
		#ifdef DEBUG
			Serial.println("START - Le bouton OEIL  start a ete appuye - La Led de l'oeil s'allume - Les autres s'eteignent");
		#endif
		digitalWrite(ledCorp1Pin, ledOff);
		digitalWrite(ledCorp2Pin, ledOff);
		digitalWrite(ledChapeauPin, ledOff);
		digitalWrite(ledOeilPin, ledOn);
		buttonPressed=0;
		//sStateNext=ATT_5_SEC;
	}
	else if (buttonChapeauDStartState == 0) {
		#ifdef DEBUG
			Serial.println("START - Le bouton CHAPEAU  start a ete appuye - La Led du chapeau s'allume - Les autres s'eteignent");
		#endif
		digitalWrite(ledCorp1Pin, ledOff);
		digitalWrite(ledCorp2Pin, ledOff);
		digitalWrite(ledChapeauPin, ledOn);
		digitalWrite(ledOeilPin, ledOff);
		buttonPressed=1;
		//sStateNext=ATT_5_SEC;
	}
	/*else {
		#ifdef DEBUG
			Serial.println("START - Waiting for Start button to be pressed - Corp led on");
		#endif
		digitalWrite(ledCorp1Pin, ledOn);
		digitalWrite(ledCorp2Pin, ledOn);
		sStateNext=DEPART; //notneeded ?
	}
	*/
	return buttonPressed;
}

void wait(int tmp){
	if(buttonPressed == 0){
		digitalWrite(ledChapeauPin, ledOff);
		for(int i=0; i<5; i++){
		    digitalWrite(ledOeilPin, ledOn);
			delay(tmp/10);
			digitalWrite(ledOeilPin, ledOff);
			delay(tmp/10);
		}
	}
	else if(buttonPressed == 1){
	    digitalWrite(ledOeilPin, ledOff);
		for(int i=0; i<5; i++){
		    digitalWrite(ledChapeauPin, ledOn);
			delay(tmp/10);
			digitalWrite(ledChapeauPin, ledOff);
			delay(tmp/10);
		}
	}
}

void rotationJupe(){
	#ifdef DEBUG
		Serial.println("JUPE - Mise en route de la jupe");
	#endif
	digitalWrite(moteurJupe5V, 1);
}

void rotationJupeAleatoire(){
	#ifdef DEBUG
		Serial.println("JUPE - Mise en route de la jupe");
	#endif
	nbPas = nbPas + 1;
	if(nbPas == 5){	    
		digitalWrite(moteurJupe5V, 0);
		digitalWrite(moteurJupeGND, 1);
		delay(400);
		nbPas = 0;
		digitalWrite(moteurJupe5V, 1);
		digitalWrite(moteurJupeGND, 0);
		}
}


//fonction tempo servo Direction
void tempoD (long tempoBase){
	long difAngleDir;
	long tempoDir; 
	difAngleDir = (tabDirPied[posActuelPied][dirSouhaitePied] ) - (tabDirPied[!posActuelPied][dirActuelPied]);
	difAngleDir = abs(difAngleDir);
	#ifdef DEBUG
		Serial.print("difAngleDir");
		Serial.println(difAngleDir);
 	#endif
	tempoDir = ((difAngleDir * tempoBase)/ 90);
  	#ifdef DEBUG
	    	Serial.print("Direction actuel du pied ");
	    	Serial.println(dirActuelPied);
	    	Serial.print(" Direction souhaite du pied ");
	    	Serial.println(dirSouhaitePied);
	    	Serial.print(" Tempo Direction ");
		Serial.println(tempoDir);
  	#endif
  	delay(tempoDir);
}

void avance(int directionSouhaite){
	dirSouhaitePied = directionSouhaite;
  	servoD.write(tabDirPied[posActuelPied][directionSouhaite]);
  	#ifdef DEBUG
 	   	Serial.print("dir pied ");
   		Serial.print(directionSouhaite);
    		Serial.print("position pied ");
    		Serial.println(posActuelPied);
  	#endif
  	tempoD(tempoServoD);    // delais si changement de direction de court si 90° à long si 270° 
  	dirActuelPied = dirSouhaitePied;
  	servoM.write(tabPosPied[!posActuelPied]);
  	posActuelPied =! posActuelPied;
  	#ifdef DEBUG
   		Serial.print("position pied ");
	    	Serial.println(posActuelPied);
  	#endif
  	delay(tempoServoM);
  
  	#ifdef DEBUG
	    	Serial.print("dir pied ");
	    	Serial.print(tabDirPied[posActuelPied][directionSouhaite]);
	     	Serial.print(" ");
	    	Serial.println(dirActuelPied);
  	#endif
}

void tourne(int angleSouhaite) {		// max 35° ideal entre 5° et 25° (10)
	int angleInitial;
	int anglePlusX;
	int tempoTourne;
	angleInitial = tabDirPied[posActuelPied][dirActuelPied];
	anglePlusX = angleInitial + angleSouhaite;
	tempoTourne = ((tempoServoD / 90) * angleSouhaite);
	#ifdef DEBUG
		Serial.print("angleInitial");
		Serial.println(angleInitial);
		Serial.print("angleSouhaite");
		Serial.println(angleSouhaite);
		Serial.print("anglePlusX");
		Serial.println(anglePlusX);
		Serial.print("tempoTourne");
		Serial.println(tempoTourne);
	#endif
	servoM.write(posCentrePied); 	// on met le servo de marche (servoM) dans la position basse
	delay(tempoServoM / 2);
	servoD.write(anglePlusX);		// on met le servo de direction (servoD)  + X° de sa position actuel
	delay(tempoTourne);
	servoM.write(tabPosPied[posActuelPied]);	// on remet le servo de marche (servoM) dans la posistion en debut de fonction tourne
	delay(tempoServoM / 2);
	servoD.write(angleInitial);		//on remet le pied a l'origine : on remet le servo de direction (servoD) dans sa position initial
	delay(tempoTourne);
}

int attaqueAveugle() {
	// retourne 5 ou 6 (Attaque ou recherche_adv)
	/* 	- on commence par devant ou gauche en fonction du bouton apuyer pour le demarrage
  		- avance(devant) ou gauche
 		- apeler la fonction detect -> si Vrai : passe a l'ETAT Attaque (avec direction de detec)
  		- avance(devant) ou arriere
  		- apeler la fonction detect -> si vrai : idem
  		et on boucle pendant 3 pas de chaque cote sauf si detection (si on a implementer les interruptions)
  		- puis on passe a l'ETAT CHERCHE_ADV
  	*/
  	int etatSuivant = CHERCHE_ADV;
	if(buttonPressed == 0){
		for(int i=0; i<4; i++){
		    avance(DIR_GAUCHE);
			avance(DIR_ARRIERE);
		    if(detection()){
				etatSuivant=ATTAQUE;
				break;
		    }
		}
	}
	else{
		for(int i=0; i<4; i++){
		    avance(DIR_AVANT);
			avance(DIR_DROITE);
		    if(detection()){
				etatSuivant=ATTAQUE;
				break;
		    }
		}
	}
	return etatSuivant;
}

void loop() {
	switch (sState) {
		case DEPART:
			#ifdef DEBUG
        			Serial.println("DEPART");
			#endif
        	digitalWrite(ledChapeauPin, ledOn);
			digitalWrite(ledOeilPin, ledOn);
			buttonPressed=identifyButtonPress();
			sStateNext=ATT_5_SEC; 
		break;
		case ATT_5_SEC:
			#ifdef DEBUG
				Serial.println("ATT_5_SEC");
			#endif
			wait(4900);
			sStateNext=JUPE;
		break;
		case JUPE:
			#ifdef DEBUG
				Serial.println("JUPE DEPART");
			#endif
			rotationJupe();
			digitalWrite(ledCorp1Pin, ledOn);
			sStateNext=ATTAQUE_AVEUGLE;
		break;
		case ATTAQUE_AVEUGLE:
			#ifdef DEBUG
				Serial.println("ATTAQUE_AVEUGLE");
			#endif
			digitalWrite(ledOeilPin, ledOn);
			sStateNext=attaqueAveugle();
		break;
		case ATTAQUE:
			#ifdef DEBUG
				Serial.println(" ATTAQUE");
			#endif
			/*	- Avance x pas
				- detect
			*/
			digitalWrite(ledOeilPin, ledOn);	
			digitalWrite(ledChapeauPin, ledOff);
			for(int i=0; i<2; i++){
				avance(capteurQuiDetect); 
				rotationJupeAleatoire();
			}
			digitalWrite(ledOeilPin, ledOff);	
			digitalWrite(ledChapeauPin, ledOn);
			detect=detection();
			if (detect == 0) {
				sStateNext=CHERCHE_ADV;
			}
		break;
		case CHERCHE_ADV:
			#ifdef DEBUG
				Serial.println("CHERCHE_ADV");
			#endif
			digitalWrite(ledOeilPin, ledOff);	
			digitalWrite(ledChapeauPin, ledOn);
			tourne(10);
			detect=detection();
			/* if find someone ATTAQUE*/
			if (detect == 1) {
				sStateNext=ATTAQUE;
			}
			else{
				sStateNext=CHERCHE_ADV;
			}
			
		break;
	}
	sState = sStateNext;
}

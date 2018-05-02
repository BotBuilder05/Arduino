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
const int moteurJupe = 5;

const int ledOeilPin=11;
const int ledChapeauPin=4;
const int ledCorp1Pin=7;
const int ledCorp2Pin=10;

int s_state , s_state_next;
int detect=0;
int buttonPressed;
//variable pour les LED
#define ledOn 0
#define ledOff 1

//variable pour les servos et la fonction avance
#define SERVO_D 3		//pin du servoD
#define SERVO_M 9		//pin du servoM

#include <Servo.h>
Servo servoD;  // creation d'un objet servo pour controler un servo
Servo servoM;

const int DIR_AVANT = 2;   // direction souhaité
const int DIR_DROITE = 1;
const int DIR_ARRIERE = 0;
const int DIR_GAUCHE = 3;

int Tab_Dir_Pied[2][4]={
  {67,112,157,22},
  {157,22,67,112}
};

const int HAUTE_AVANT = 0;
const int HAUTE_ARRIERE = 1;
boolean Pos_Actuel_pied = HAUTE_AVANT ; // memorise la position HAUTE_AVANT ou HAUTE_ARRIERE du pied

int Tab_Pos_Pied[2] = {10,170};
int Tempo_Servo_D = 700;
int Tempo_Servo_M = 700;

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
 	servoM.write(Tab_Pos_Pied[Pos_Actuel_pied]);
	servoD.write(Tab_Dir_Pied[Pos_Actuel_pied][DIR_AVANT]);
	// On initialise la machine d'etat 
	s_state=DEPART;
}

int detection() {
	int detectStatus;
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
	return detectStatus;
}

void tourne(int Angle_Souhaite) {
	/*
  	- on met le servo de marche (servoM) dans la position basse
  	- on met le servo de direction (servoD) line 84 de etat marche +10 
  	- on met le servo de marche (servoM) dans la posistion en debut de fonction tourne 10
  	- on remet le pied a l'origine on met le servo de direction (servoD) line 84 de etat marche:w
  	*/
	return 0;
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
		s_state_next=ATT_5_SEC;
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
		s_state_next=ATT_5_SEC;
	}
	else {
		#ifdef DEBUG
			Serial.println("START - Waiting for Start button to be pressed - Corp led on");
		#endif
		digitalWrite(ledCorp1Pin, ledOn);
		digitalWrite(ledCorp2Pin, ledOn);
		s_state_next=DEPART; //notneeded ?
	}
	return buttonPressed;
}

void wait(){
	delay(4900);
}

void rotationJupe(){
	#ifdef DEBUG
		Serial.println("JUPE - Mise en route de la jupe");
	#endif
	digitalWrite(moteurJupe, 1);
}


void avance(int Direction_Souhaite){
  servoD.write(Tab_Dir_Pied[Pos_Actuel_pied][Direction_Souhaite]);
  #ifdef DEBUG
    Serial.print("dir pied ");
    Serial.print(Direction_Souhaite);
     Serial.print(" POs ");
    Serial.println(Pos_Actuel_pied);
  #endif
  delay(Tempo_Servo_D); // delais si position pas bonne court si 90 ou -90 long si 180 a faire!!
  servoM.write(Tab_Pos_Pied[Pos_Actuel_pied]);
  #ifdef DEBUG
    Serial.print("position pied ");
    Serial.println(Pos_Actuel_pied);
  #endif
  Pos_Actuel_pied =! Pos_Actuel_pied;
  #ifdef DEBUG
    Serial.print("position OPO pied ");
    Serial.println(Pos_Actuel_pied);
  #endif
  delay(Tempo_Servo_M);
  
  #ifdef DEBUG
    Serial.print("position pied ");
    Serial.print(Tab_Dir_Pied[Pos_Actuel_pied][Direction_Souhaite]);
     Serial.print(" ");
    Serial.println(Pos_Actuel_pied);
  #endif
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
	avance(1);
	detection();
	avance(0);
	detection();
}

void loop() {
	switch (s_state) {
		case DEPART:
			#ifdef DEBUG
        			Serial.println("DEPART");
			#endif
			buttonPressed=identifyButtonPress();
			s_state_next=ATT_5_SEC; 
		break;
		case ATT_5_SEC:
			#ifdef SLOW
				delay(attente);
			#endif
			#ifdef DEBUG
				Serial.println("ATT_5_SEC");
			#endif
			wait();
			s_state_next=JUPE;
		break;
		case JUPE:
			#ifdef DEBUG
				Serial.println("JUPE DEPART");
			#endif
			// rotationJupe();
			s_state_next=ATTAQUE_AVEUGLE;
		break;
		case ATTAQUE_AVEUGLE:
			#ifdef DEBUG
				Serial.println("ATTAQUE_AVEUGLE");
			#endif
			attaqueAveugle();
			s_state_next=CHERCHE_ADV;
		break;
		case ATTAQUE:
			#ifdef DEBUG
				Serial.println(" ATTAQUE");
			#endif
			/*	- Avance x pas
				- detect
			*/
			avance(1); // 1 & changer par la valeur de la direcion (variable)
			detect=detection();
			if (detect == 0) {
				s_state_next=CHERCHE_ADV;
//manu
			}
		break;
		case CHERCHE_ADV:
			#ifdef DEBUG
				Serial.println("CHERCHE_ADV");
			#endif
//manu founion tourne			tourne(10);
// xavier fonction detection			detect=detection();
			/* if find someone ATTAQUE*/
			if (detect == 1) {
				s_state_next=ATTAQUE;
			}
			
		break;
	}
	s_state = s_state_next;
}

// Variable Machine d'etat
#define DEBUG
#define DEBUG_DETECTER
#define DEBUG_CALIBRATION_IR
#define DEBUG_WAIT
//#define DEBUG_VOLET
//#define DEBUG_LECTURE_CAPTEUR
//#define DEBUG_DETECTER_SEUIL
//#define SLOW 
//#define DEBUG_CALIBRATION_MAX

#define START 1
#define WAIT 2
#define FLAPS 3
#define BLINDATTACK 4
#define DETECTION 5
#define AVANCE 6
#define RECULE 7
#define TOURNE 8

int s_state, s_state_next, s_state_previous;

// Variable Gestion moteur
#define in1Pin 11  /* Moteur 1*/
#define in2Pin 12  /* Moteur 1*/
#define in3Pin 9  /* Moteur 2/Gauche*/
#define in4Pin 10 /* Moteur 2/Gauche*/

int moteur1[3] = {in1Pin, in2Pin},
    moteur2[3] = {in3Pin, in4Pin};
int speedPinMoteur1 = 5;
int speedPinMoteur2 = 6;

int buttonPressed;

// Variable to calibrate IR capteur
#define capteurLF A0
#define capteurLR A1

#define capteurIRAvant A3
#define capteurIRArriere A2
int detection = 0; //1 capteur avant ; 2 capteur arriere ; 0 init calue
int status_detection; //flag de detection
// pointeur sur un entier pour acceder aux valeurs du tableau des valeurs de calibration
int *cal;


// LED
const int led1Pin = 7; //LED rouge
const int led2Pin = 8; //LED verte


// Bouton
const int buttonRedStartPin = 3;
const int buttonJauneStartPin = 2;

// Attente
int attente =3000;

// Servo
// ne pas utiliser pin 9 et 10 pwm a cause du timer
// Import biblio gestion servo-moteur
#include <Servo.h>
//creation de l'objet servo
Servo servo;
const int servoPin=4; // VALEUR A VERIFIER


#define VOLETS_LOCKED 45
#define VOLETS_OPENED 0


void setup() {  
	#ifdef DEBUG
		Serial.begin(9600 );
	#endif
	#ifdef DEBUG_SEUIL
		Serial.begin(9600 );
	#endif

	//Inititalise les LED a OFF
	#ifdef DEBUG
  		Serial.println("SETUP : On eteint les leds");
	#endif
	digitalWrite(led1Pin, 0);
	digitalWrite(led2Pin, 0);

	// Met la LED rouge a ON
	#ifdef DEBUG
		Serial.println("SETUP : On allume la led ROUGE");
	#endif
	digitalWrite(led1Pin, 1);

	// Servo
	// Lien Pin / Servo
	servo.attach(servoPin);
	#ifdef DEBUG
  		Serial.println("SETUP : On bloque les volets");
	#endif
	servo.write(VOLETS_LOCKED);

	// On initialise les moteurs
	pinMode(speedPinMoteur1, OUTPUT);
	pinMode(speedPinMoteur2, OUTPUT);
	pinMode(moteur1[0], OUTPUT);
	pinMode(moteur1[1], OUTPUT);
	pinMode(moteur2[0], OUTPUT);
	pinMode(moteur2[1], OUTPUT);

	// On initialise la machine d'etat 
	s_state=START;
}

int identifyButtonPress() {
	// 0 -> bouton droite
	// 1 -> bouton gauche // oeil -> led oeil 
	// Tant que vrai, lecture bouton 1 lecture bouton 2
	int buttonRouge=0;
	int buttonJaune=0;

	while ((buttonRouge == 0) && (buttonJaune == 0)) {
		buttonRouge = digitalRead(buttonRedStartPin);
		buttonJaune = digitalRead(buttonJauneStartPin);
    		//filtrée ici le rebond...
    		#ifdef DEBUG
      			Serial.print("START - Etat bouton Rouge: ");
      			Serial.println(buttonRouge);
      			Serial.print("START - Etat bouton Jaune: ");
      			Serial.println(buttonJaune);
    		#endif
	}
	if (buttonRouge == 1){
		#ifdef DEBUG
			Serial.println("START - Le bouton Rouge  start a ete appuye - La Led verte s'allume - La rouge s'eteint");
		#endif
	  	digitalWrite(led1Pin, 0);
	  	digitalWrite(led2Pin, 1);
		buttonPressed=0;
		s_state_next=WAIT;
	}
	else if (buttonJaune == 1) {
		#ifdef DEBUG
			Serial.println("START - Le bouton Jaune  start a ete appuye - La Led rouge s'allume - La verte s'eteint");
		#endif
	  	digitalWrite(led1Pin, 0);
	  	digitalWrite(led2Pin, 1);
		delay(10);
	  	digitalWrite(led1Pin, 1);
	  	digitalWrite(led2Pin, 0);
		buttonPressed=1;
		s_state_next=WAIT;
	}
	/*else {
		#ifdef DEBUG
			Serial.println("START - Waiting for Start button to be pressed - Corp led on");
		#endif
		digitalWrite(ledCorp1Pin, ledOn);
		digitalWrite(ledCorp2Pin, ledOn);
		s_state_next=DEPART; //notneeded ?
	}
	*/
	return buttonPressed;
}

int * lectureCapteur(){

	//tableau retournant les valeurs lues pour avant et arriere
	static int valeursLu[2];

	// Lecture capteur
	valeursLu[0] = analogRead(capteurIRAvant);
	valeursLu[1] = analogRead(capteurIRArriere);

	#ifdef DEBUG_LECTURE_CAPTEUR
		Serial.print("Valeur Capteur Avant");
		Serial.println("  Valeur Capteur Arriere");
		Serial.print("       ");
		Serial.print(valeursLu[0]);
		Serial.print("       ");
		Serial.println(valeursLu[1]);
	#endif
  
	return valeursLu;
}

int * valeurMoyCal(int nbLecture) {
  // ajouter un parametre a la fonction pour avoir 10 valeurs pendant l'init et 5 valeurs pendant le combat 
	int *valeurLu;
	static int valeurMoy[2];
    
	for(int z=0;z<nbLecture;z++){
		valeurLu=lectureCapteur();
		valeurMoy[0] += *(valeurLu);
		valeurMoy[1] += *(valeurLu + 1);
		delay(50);
	}
	valeurMoy[0] = valeurMoy[0] / nbLecture;
	valeurMoy[1] = valeurMoy[1] / nbLecture;

	return valeurMoy;
}

int detecter() {
	// On lit les valeurs sur les capteurs Avant et arriere
	//int seuil = 60; //Valeur a 40cm : 60
	int seuil[2];
	int maxarray=sizeof(seuil)/sizeof(*seuil); //pour avoir le nb elment ds le tableau
	for (int i=0;i<maxarray ;i++) {
		seuil[i]=cal[i];
	}

	// TODO To adjust contest's day
	//Seuil avant
	//seuil[0]=110;
	if (cal[0] < 100) {
		seuil[0]=100;
	}
	//Seuil arriere
	//seuil[1]=130;
	if (cal[1] < 100) {
		seuil[1]=100;
	}

	/*// Pointeur sur un entier pour acceder au valeur du tableau de valeurs lues
	int *valeurLu;
	valeurLu=lectureCapteur();*/
	int *valeurMoy;
	valeurMoy=valeurMoyCal(5);

	detection = 0;
    
	#ifdef DEBUG_DETECTER
		Serial.print("Seuil1   : ");
		Serial.print(*(seuil));
		Serial.print("       ");
		Serial.println(*(seuil+1));
		Serial.print("valeurMoy: ");
		Serial.print(*(valeurMoy));
		Serial.print("       ");
		Serial.println(*(valeurMoy+1));
	#endif
	#ifdef DEBUG_DETECTER_SEUIL
		Serial.print("valeurMoy: ");
		Serial.print(*(valeurMoy));
		Serial.print("       ");
		Serial.println(*(valeurMoy+1));
	#endif
	// si la valeur(moyenne) capteur avant superieure au seuil avant et si la valeur(moyenne) capteur avant est supperieurie a la valeur du capteur arriere
	if (( *(valeurMoy) > *(seuil)) && (*(valeurMoy) > *(valeurMoy + 1))) {
		detection = 1;
	}
	// si la valeur(moyenne) capteur arriere superieure au seuil arriere et si la valeur(moyenne) capteur arriere est supperieure a la valeur du capteur avant
	else if (( *(valeurMoy + 1) > *(seuil+1)) && (*(valeurMoy + 1) > *(valeurMoy))) {
		detection = 2;
	}

	status_detection=1;
	return detection;
}

int * calibrationIR(int nbLecture){
	// Pointeur sur un entier pour acceder au valeur du tableau de valeurs lues
	int *valeurLu;
	static int CalibrationIR[2];

	#ifdef DEBUG_CALIBRATION_IR
		Serial.print("WAIT1 - Valeur Capteur Avant");
		Serial.println("  Valeur Capteur Arriere");
	#endif
	// Lecture a blanc pour virer eventuelle valeur aberente
	// TODO Veirifier si ces analoguRead sont necessaire avec la double alim!
	//analogRead(capteurIRAvant);
	//analogRead(capteurIRArriere);
	for(int z=0;z<nbLecture;z++){
		valeurLu=lectureCapteur();
		CalibrationIR[0] += *(valeurLu);
		CalibrationIR[1] += *(valeurLu + 1);
		#ifdef DEBUG_CALIBRATION_IR
			Serial.print(*(valeurLu));
			Serial.print(" ");
			Serial.println(*(valeurLu + 1));
		#endif
		delay(100);
	}
	CalibrationIR[0] = CalibrationIR[0] / nbLecture;
	CalibrationIR[1] = CalibrationIR[1] / nbLecture;

	#ifdef DEBUG_CALIBRATION_IR
		Serial.print("WAIT2 - Calibration capteur Avant  : ");
		Serial.println(CalibrationIR[0]);
		Serial.print("Calibration capteur Arriere: ");
		Serial.println(CalibrationIR[1]);
	#endif

	return CalibrationIR;
}

/*
int * calibrationMaxIR(){
	// Pointeur sur un entier pour acceder au valeur du tableau de valeurs lues
	int *valeurLu;
	static int CalibrationIR[2];

	#ifdef DEBUG_CALIBRATION_MAX
		Serial.print("WAIT - Valeur Capteur Avant");
		Serial.println("  Valeur Capteur Arriere");
	#endif
	for(int z=0;z<10;z++){
		valeurLu=lectureCapteur();
		if (*(valeurLu) > CalibrationIR[0]){
			CalibrationIR[0] = *(valeurLu);
		}
		if (*(valeurLu + 1) > CalibrationIR[1]){
			CalibrationIR[1] = *(valeurLu + 1);
		}
		delay(100);
	}

	#ifdef DEBUG_CALIBRATION_MAX
		Serial.print("WAIT - Calibration capteur Avant  : ");
		Serial.println(CalibrationIR[0]);
		Serial.print("Calibration capteur Arriere: ");
		Serial.println(CalibrationIR[1]);
	#endif

	return CalibrationIR;
}
*/

void wait() {
	#ifdef DEBUG_WAIT
		Serial.println("WAIT - J'attends... et je calibre mes capteurs");
	#endif
	int x=0;
	while (x<=2) {
		delay(1000);
		x+=1;
	} 
	// INUTILE LES VOLETS SONT DEVANT LES CAPTEURS!!!!!!!
	cal=calibrationIR(10);
	//cal=calibrationMaxIR();
	delay(600);
}

void Robot50HzInterrupt() {
/*
	#ifdef DEBUG
		Serial.println("Interruptions");
	#endif
	beDetect = detecter();
	if(beDetect){
		digitalWrite(led1Pin, HIGH);
	}
*/
}

void ouvertureVolet() {
	#ifdef DEBUG_VOLET
		Serial.println("FLAP - Ouverture Volets");
	#endif
	servo.write(VOLETS_OPENED);
	delay(100);
	servo.write(VOLETS_LOCKED);
}

void avance() {
	#ifdef DEBUG
		Serial.println("AVANCE : on avance");
	#endif
	digitalWrite(moteur1[0], HIGH);
	digitalWrite(moteur1[1], LOW);
	digitalWrite(moteur2[0], HIGH);
	digitalWrite(moteur2[1], LOW);
	analogWrite(speedPinMoteur1, 255);
	analogWrite(speedPinMoteur2, 255);
}

void recule() {
	#ifdef DEBUG
		Serial.println("RECULE : on recule");
	#endif
	digitalWrite(moteur1[0], LOW);
	digitalWrite(moteur1[1], HIGH);
	digitalWrite(moteur2[0], LOW);
	digitalWrite(moteur2[1], HIGH);
	analogWrite(speedPinMoteur1, 255);
	analogWrite(speedPinMoteur2, 255);
}

void tourneGauche(){
	#ifdef DEBUG
		Serial.println("TOURNE A GAUCHE ");
	#endif
	digitalWrite(moteur1[0], HIGH);
	digitalWrite(moteur1[1], LOW);
	digitalWrite(moteur2[0], LOW);
	digitalWrite(moteur2[1], HIGH);
	//for (int i; i<50; i++) { faire accelerer graduellement mettre un param a la fonction, appele lafonction tourneGauhe dans la bouble depuis la machine d'etat
		//analogWrite(speedPinMoteur1, 80+i);
		//analogWrite(speedPinMoteur2, 80+i);
		analogWrite(speedPinMoteur1, 70);
		analogWrite(speedPinMoteur2, 70);
	//}
}

void tourneDroite(){
	#ifdef DEBUG
		Serial.println("TOURNE A DROITE ");
	#endif
	digitalWrite(moteur1[0], LOW);
	digitalWrite(moteur1[1], HIGH);
	digitalWrite(moteur2[0], HIGH);
	digitalWrite(moteur2[1], LOW);
	analogWrite(speedPinMoteur1, 80);
	analogWrite(speedPinMoteur2, 80);
}

int detectionDojo() {
	int bord_circuit_analog_front=0;
	int bord_circuit_analog_rear=0;
	int dojo_limit=0:

	bord_circuit_analog_front = analogRead(capteurLF);// Black > 600 White < 180
	bord_circuit_analog_rear = analogRead(capteurLR); // Black > 600 White < 180

	int const white_limit=180;
	if (bord_circuit_analog_front < white_limit || bord_circuit_analog_rear < white_limit) {
	  dojo_limit=1;
	}
	return dojo_limit;
}

void loop() {
  switch (s_state){
	#ifdef DEBUG
		Serial.println("");
		Serial.print("State:");
		Serial.println(s_state);
		Serial.print("Next State:");
		Serial.println(s_state_next);
	#endif

    case START:
	#ifdef DEBUG
	 	Serial.println("DEPART");
	#endif

	identifyButtonPress();
	//s_state_next is set in the function
	break;

    case WAIT:
	#ifdef SLOW
		delay(attente);
	#endif
	#ifdef DEBUG
		Serial.println("WAIT");
	#endif
	wait();
	s_state_next=FLAPS;
	break;

    case FLAPS:
	#ifdef SLOW
		delay(attente);
	#endif
	#ifdef DEBUG
		Serial.println("FLAPS");
	#endif
	ouvertureVolet();
	s_state_next=BLINDATTACK;
	break;

    case BLINDATTACK:
	#ifdef SLOW
		delay(attente);
	#endif
	#ifdef DEBUG
		Serial.println("BEGIN - BLINDATTACK");
	#endif
	if (buttonPressed==0) {
		tourneDroite();
	}
	else {
		tourneGauche();
	}
	delay (500);
	avance();  // pendant 300ms a coder
	delay (30);
	#ifdef DEBUG
		Serial.println("END - BLINDATTACK");
	#endif
	// On eteint la LED verte
	digitalWrite(led1Pin, 0);
	s_state_next=DETECTION;
	break;

    case DETECTION:
	#ifdef SLOW
		delay(attente);
	#endif
	#ifdef SLOW
		Serial.println("DETECTION");
	#endif
        
	int detect;
	detect=detecter(); // lecture catpteur + tourne sur place droite ou gauche en fonction du catpeur qui detect
	#ifdef DEBUG
		Serial.print("DETECTION - Valeur de detect:");
		Serial.println(detect);
	#endif
	if (detect == 1 ) {
		// On detect devant , on eteint la led rouge on alume la verte
		digitalWrite(led1Pin, 0);
		digitalWrite(led2Pin, 1);
		//DEBUG DETECTION
		tourneGauche(); // Pour contracarer la trop grande vitesse de rotation
		delay(50);
		// END DEBUG DETECTION 
		s_state_next=AVANCE;
	}
	else if (detect == 2) {
		// On detect derriere , on eteint la led verte on alume la rouge
		digitalWrite(led1Pin, 1);
		digitalWrite(led2Pin, 0);
		//DEBUG
		tourneGauche(); // Pour contracarer la trop grande vitesse de rotation
		delay(50);
		// END DEBUG DETECTION 
		s_state_next=RECULE;
		//s_state_next=TOURNE;
	} 
	else {
		// On detect rien , on eteint toutes les led
		digitalWrite(led1Pin, 0);
		digitalWrite(led2Pin, 0);
		s_state_next=TOURNE;
	}
	break;

     case AVANCE:
	#ifdef SLOW
		delay(attente);
	#endif
	//#ifdef DEBUG
	//  Serial.println("AVANCE");
	//#endif
	avance(); // lecture capteur + moteur
	s_state_next=DETECTION;     
	break;

     case RECULE:
	#ifdef SLOW
		delay(attente);
	#endif
	//#ifdef DEBUG
	//  Serial.println("RECULE");
	//#endif
	recule(); // lecture capteur + moteur
	s_state_next=DETECTION;     
	break;

     case TOURNE:
      #ifdef SLOW
        delay(attente);
      #endif
      #ifdef SLOW
        Serial.println("TOURNE");
      #endif
      tourneDroite();
      s_state_next=DETECTION;  
      break;
  }
  s_state = s_state_next;
}

/*
# Strategie
* Mise sous tension -> initialisation servo moteur (pied en particulier pour avoir la position de depart)
* Appuie sur boutojn cote adversaire (bouton droit ou bouton gauche)
* Attendre 5 secondes
* Demarrage jupe
* Avance
  - si dojo Tourne de 10°
* Detection
  - si detection avance
  - si non Tourne 1°
*/
#define DEBUG
//#define SLOW 

//Variable Machine d'etat
#define START 1
#define WAIT 2
#define SKIRT 3
#define FORWARD 4
#define SEARCH 5

void setup() {
	#ifdef DEBUG
		Serial.begin(9600 );
	#endif
	// On initialise la machine d'etat 
	s_state=START;
}

int detectDojo() {
	/* This finction should
		* return 0 if it detects dojo
		* return 1 othewise
	This should be called by Timer interruption
	*/
	return 0
}

void loop() {
	switch (s_state) {
		case START:
			#ifdef DEBUG
        			Serial.println("DEPART");
			#endif
			identifyButtonPress();
			waitButtonPres();
		break;
		case WAIT:
			#ifdef SLOW
				delay(attente);
			#endif
			#ifdef DEBUG
				Serial.println("WAIT");
			#endif
			wait();
			s_state_next=SKIRT;
		break;
		case SKIRT:
			#ifdef DEBUG
				Serial.println("SKIRT START");
			#endif
			skirtStart();
			s_state_next=FORWARD;
		break;
		case FORWARD:
			#ifdef DEBUG
				Serial.println("Move FORWARD");
			#endif
			/* If Dojo tourne 10)*/
			if (detect_dojo == 0) {
				s_state_next=TURN;
			}
			else {
				/*else we need to search the other*/
				s_state_next=SEARCH;
			}
		break;
		case TURN:
			#ifdef DEBUG
				Serial.println("TURN");
			#endif
			/* Move the foot in order to turn the robot*/
			/* if detect_dojo=0 then turn=10° otherwise turn=10°)*/
			if (detect_dojo == 0) {
				turn(10);
			}
			else {
				turn(1);
			}
		break;
		case SEARCH:
			#ifdef DEBUG
				Serial.println("SEARCH");
			#endif
			detect();
			/* if find someone forward*/
			if (detect == 0) {
				s_state_next=FORWARD;
			}
			else {
				/*else tourne 1°*/
				s_state_next=TURN;
			}
			
		break;
	}
	s_state = s_state_next;
}

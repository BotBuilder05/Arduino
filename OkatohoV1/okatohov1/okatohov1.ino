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

int s_state , s_state_next;
int detect=0;
int buttonPressed;

void setup() {
	#ifdef DEBUG
		Serial.begin(9600 );
	#endif
	// On initialise la machine d'etat 
	s_state=DEPART;
}

int detection() {
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
	return 0;
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
	// 1 -> bouton gauche
	// Tant que vrai, lecture bouton 1 lecture bouton 2
}
void wait(){
	delay(4900);
}
void rotationJupe(){
}

void avance(int Direction_Souhaite){
	// a recupere de etat_marche
}

int attaqueAveugle() {
	// retourne 5 ou 6 (Attaque ou recherche_adv
}

void loop() {
	switch (s_state) {
		case DEPART:
			#ifdef DEBUG
        			Serial.println("DEPART");
			#endif
			buttonPressed=identifyButtonPress();
			s_state_next=ATT_5_SEC; 
//xavier
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
			rotationJupe();
			s_state_next=ATTAQUE_AVEUGLE;
//xaveir
		break;
		case ATTAQUE_AVEUGLE:
			#ifdef DEBUG
				Serial.println("ATTAQUE_AVEUGLE");
			#endif
			/* 	- on commence par devant ou gauche en fonction du bouton apuyer pour le demarrage
  				- avance(devant) ou gauche
 				- apeler la fonction detect -> si Vrai : passe a l'ETAT Attaque (avec direction de detec)
  				- avance(devant) ou arriere
  				- apeler la fonction detect -> si vrai : idem
  				et on boucle pendant 3 pas de chaque cote sauf si detection (si on a implementer les interruptions)
  				- puis on passe a l'ETAT CHERCHE_ADV
  			*/
			s_state_next=attaqueAveugle();
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

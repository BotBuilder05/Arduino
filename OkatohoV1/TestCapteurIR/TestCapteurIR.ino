/*
Pin cateur distance A6 (avant)
Pin cateur distance A2 (droit)
Pin cateur distance A3 (arriere)
Pin cateur distance A1 (gauche)
*/

const int capteurIRavant = A6; 
const int capteurIRdroit = A3; 
const int capteurIRarriere = A1; 
const int capteurIRgauche = A2; 

int capteurList[4] = {capteurIRavant, capteurIRdroit, capteurIRarriere, capteurIRgauche};
int maxarray=sizeof(capteurList)/sizeof(*capteurList);

void setup() {
	Serial.begin(115200);
    
	pinMode(capteurIRavant, INPUT);
	pinMode(capteurIRdroit, INPUT);
	pinMode(capteurIRarriere, INPUT);
	pinMode(capteurIRgauche, INPUT);
  
}

void lectureCapteur(int capteur) {
	int valeurLue=0;

	valeurLue = analogRead(capteur);
	Serial.print("Avant: ");
	Serial.println(valeurLue);
	delay(500);
}

void loop() {
	for (int i=0;i<maxarray ;i++) {
		lectureCapteur(i);
	}
}

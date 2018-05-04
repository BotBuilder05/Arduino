#define DEBUG

const int capteurIR = A2;   //capteur IR1 Ligne blanche 
int valeurLue; //debug
int detection;
int status_detection; //flag de detection

const int servoPin = 9;          //PIN de la led à changer
const int ledPin = 4;          //PIN de la led à changer
int beDetect;

#define POSITION_POSEE 20
#define POSITION_HAUTE 47 
const int FILTRE_IR=20;

int moyen1, moyen2;

void setup() {
	// initialize serial communication at 115200 bits per second:
	Serial.begin(9600);
	detection = 0;
	status_detection=0; //flag de detection
}

	static int valeurLueBrut[20];
	static int valeurAcc[20];

int detecter() {
	// lecture capteur frontal
	// Detect de 30 a 800 - valeur capteur (à confirmer)

/*	for(int bg = 0 ; bg < FILTRE_IR ; bg++){
                valeurLueBrut[bg]=0;
                valeurAcc[bg]=0;
        }
*/
	//variable pour stocker la valeur lue après conversion

	valeurLueBrut[0] = analogRead(capteurIR);
        #ifdef DEBUG
                Serial.print("Valeur Brut de 0 : ");
                Serial.println(valeurLueBrut[0]);
        #endif
	valeurAcc[0] = abs(valeurLueBrut[0] - valeurLueBrut[1]);
        #ifdef DEBUG
                Serial.print("Valeur Acc de 0: ");
                Serial.println(valeurAcc[0]);
        #endif
      int tmp;
	//valeurAcc =0;
	for(int bg = 1 ; bg < FILTRE_IR ; bg++){
                /*Serial.print("FILTRE_IR-bg: ");
                Serial.println(FILTRE_IR-bg);
                Serial.print("FILTRE_IR-bg-1: ");
                Serial.println(FILTRE_IR-bg-1);
                Serial.print("Valeur Brut de 0 : ");
                Serial.println(valeurLueBrut[0]);*/
		valeurLueBrut[FILTRE_IR-bg] = valeurLueBrut[FILTRE_IR-bg-1];    
                /*tmp=FILTRE_IR-bg-1;
                Serial.print("tmp : ");
                Serial.print(tmp);
                Serial.print(" ");
                Serial.print("bg : ");
                Serial.println(bg);
                Serial.print("valeurLueBrut[FILTRE_IR-bg-1]: ");
                Serial.println(valeurLueBrut[tmp]);*/

		valeurAcc[FILTRE_IR-bg] = valeurAcc[FILTRE_IR-bg-1];
	}
        #ifdef DEBUG
                Serial.print("ValeurLueBrut: ");
	        for(int bg = 0 ; bg < FILTRE_IR ; bg++){
                        Serial.print(valeurLueBrut[bg]);
                        Serial.print(" ");
        	}
                Serial.println(" ");
                Serial.print("ValeurAcc: ");
	        for(int bg = 0 ; bg < FILTRE_IR ; bg++){
                        Serial.print(valeurAcc[bg]);
                        Serial.print(" ");
        	}
                Serial.println(" ");
        #endif
      
	for(int bm = 0 ; bm < 9 ; bm++){
		moyen1=valeurAcc[bm]+valeurAcc[bm+1];
	}
	moyen1=moyen1/10;
        #ifdef DEBUG
                Serial.print("Valeur moyenne1: ");
                Serial.println(moyen1);
        #endif
     
	for(int bm = 10 ; bm < 19 ; bm++){
		moyen2=valeurAcc[bm]+valeurAcc[bm+1];
	}
	moyen2=moyen2/10;
        #ifdef DEBUG
                Serial.print("Valeur moyenne2: ");
                Serial.println(moyen2);
        #endif

	valeurLue=abs(moyen1-moyen2);
        #ifdef DEBUG
                Serial.print("Valeur moyenne: ");
                Serial.println(valeurLue);
        #endif
        #ifdef DEBUG
                Serial.println("----------------");
        #endif
  
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
}

void loop() {
	beDetect=detecter();
/*
	#ifdef DEBUG
		Serial.print("Detection : ");
		Serial.println(beDetect);
	#endif
*/
}

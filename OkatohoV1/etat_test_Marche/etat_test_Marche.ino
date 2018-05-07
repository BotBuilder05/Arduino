
#define DEBUG

#define DEPART 0
#define ATT_5_SEC 1
#define ATTAQUE_AVEUGLE 2
#define CHERCHE_ADV 3
#define ON_ATTAQUE 4
#define AVANCE 5

#define TEST_SERVO 6

#define SEUIL_DETECTION_ADV 100

#define SERVO_D 3
#define SERVO_M 9

#include <Servo.h>
//#include <arduino.h>
Servo servoD;  // create servo object to control a servo
// twelve servo objects can be created on most boards
Servo servoM;

int capteurIRavant = A6;    // select the input pin for the potentiometer
int capteurIRarriere = A1;    // select the input pin for the potentiometer
int capteurIRgauche = A2;    // select the input pin for the potentiometer
int capteurIRdroit = A3;    // select the input pin for the potentiometer

const int ledChap = 4;
const int ledcorp1 = 7;
const int ledcorp2 = 10;
const int ledoeil = 11;

int bouton_droit_oeil = 14;
int bouton_gauche = 12;
int etatbout_droit_oeil;
int etatbout_gauche;

int posd = 0;    // variable to store the servo position
int posm = 0;

int mot_jupe_p = 5;
int mot_jupe_gnd = 6;


int s_state, s_state_next;

#define POS_HAUTE_A 5
#define POS_HAUTE_B 176
#define POS_BASSE 85
int pos_m_0_180;
/*#define DIRECTION_AVANT 67
#define DIRECTION_DROITE 112
#define DIRECTION_ARRIERE 157
#define DIRECTION_GAUCHE 22*/

const int POS_AVANT = 0;
const int POS_ARRIERE = 1;

int Tab_Pos_Pied[2] = {10,170};

const int DIR_AVANT = 0;
const int DIR_GAUCHE = 1;
const int DIR_ARRIERE = 2;
const int DIR_DROITE = 3;

int Tab_Dir_Pied[2][4]={
  {67,112,157,22},
  {157,22,67,112}
};
int Dir_Actuel_Pied ;
int Dir_Souhaite_pied ;

const int HAUTE_AVANT = 0;
const int HAUTE_ARRIERE = 1;
boolean Pos_Actuel_pied = 0 ;
int tempoServoD = 700;
int tempoServoM = 800;

int tab_sensor0[180];
int val_max;
int degree_adv = 0;

// valeur absolu
int absolu(int valeur){
  if(valeur <= 0){
    valeur = (valeur * (-1));
    return valeur;
  }
}

//fonction tempo servo Direction
void tempoD (int tempo_base){
  int tempo_dir = (((Dir_Souhaite_pied - Dir_Actuel_Pied)/90*tempo_base));
  //if(tempo_dir <=0){tempo_dir=tempo_dir*-1;}
  tempo_dir = absolu(tempo_dir);
  #ifdef DEBUG
    Serial.print("Direction actuel du pied ");
    Serial.println(Dir_Actuel_Pied);
    Serial.print(" Direction souhaite du pied ");
    Serial.println(Dir_Souhaite_pied);
    Serial.print(" Tempo Direction ");
    Serial.println(tempo_dir);
  #endif
  delay(tempo_dir);
}

void Avance(int Direction_Souhaite){
  Dir_Souhaite_pied = Tab_Dir_Pied[Pos_Actuel_pied][Direction_Souhaite];
  servoD.write(Dir_Souhaite_pied);
  #ifdef DEBUG
    Serial.print("dir pied ");
    Serial.print(Direction_Souhaite);
    Serial.print(" POs ");
    Serial.println(Pos_Actuel_pied);
  #endif
  tempoD(tempoServoD); // delais si position pas bonne court si 90 ou -90 long si 180
  servoM.write(Tab_Pos_Pied[Pos_Actuel_pied]);
  #ifdef DEBUG
    Serial.print("position pied ");
    Serial.println(Pos_Actuel_pied);
  #endif
  Dir_Actuel_Pied = Tab_Dir_Pied[Pos_Actuel_pied][Direction_Souhaite];
  Pos_Actuel_pied =! Pos_Actuel_pied;
  #ifdef DEBUG
    Serial.print("position OPO pied ");
    Serial.println(Pos_Actuel_pied);
  #endif
  delay(tempoServoM);
  //Dir_Actuel_Pied = Tab_Dir_Pied[Pos_Actuel_pied][Direction_Souhaite];
  #ifdef DEBUG
    Serial.print("position pied ");
    Serial.print(Tab_Dir_Pied[Pos_Actuel_pied][Direction_Souhaite]);
     Serial.print(" ");
    Serial.println(Pos_Actuel_pied);
  #endif
}

void init_robot() {
    // Position initial
    // pied en haut
    #ifdef DEBUG
      Serial.println("init pied robot en Haut Devant");
    #endif
    servoM.write(POS_BASSE);
    delay(400);
    servoD.write(Tab_Dir_Pied[0][DIR_AVANT]);
    Dir_Actuel_Pied = Tab_Dir_Pied[0][DIR_AVANT];
     #ifdef DEBUG
      Serial.println("Fin init");
    #endif
}

void setup() {
  // put your setup code here, to run once:
  // initsystem
 Serial.begin(9600);
 
 servoM.attach(SERVO_M,850,2220);  // attaches the servo on pin 9 to the servo object
 servoD.attach(SERVO_D,700,2700);

 pinMode(bouton_droit_oeil, INPUT_PULLUP);
 digitalWrite(bouton_droit_oeil,HIGH );
 pinMode(bouton_gauche, INPUT);
 pinMode(ledChap, OUTPUT);
 pinMode(ledcorp1, OUTPUT);
 pinMode(ledcorp2, OUTPUT);
 pinMode(ledoeil, OUTPUT);

 pinMode(mot_jupe_p, OUTPUT);
 pinMode(mot_jupe_gnd, OUTPUT);
 digitalWrite(mot_jupe_gnd,LOW);
 digitalWrite(mot_jupe_p,LOW);
  pinMode(capteurIRavant, INPUT);
  pinMode(capteurIRdroit, INPUT);
  pinMode(capteurIRgauche, INPUT);
  pinMode(capteurIRarriere, INPUT);


   pos_m_0_180=POS_BASSE;
  init_robot();
  
    s_state = DEPART;
}

void loop() {
  // put your main code here, to run repeatedly:

  switch(s_state){

    case DEPART:
    	digitalWrite(ledoeil, 1);
      #ifdef DEBUG
    	  Serial.println("DEPART");
      #endif
      s_state_next=AVANCE;
      #ifdef DEBUG
        Serial.println(s_state);
      #endif
    	/*degree_adv = 0;
      	if(digitalRead(!bouton_droit_oeil)) {
      		//s_state_next = ATT_5_SEC;
      		//Serial.println("Attendre 5sec.");
      		s_state_next = TEST_SERVO;
      		Serial.println("TEST_SERVO");
      	}*/
    break;
    case AVANCE:
      while (true){
        delay(1000);
        //Avance(DIR_GAUCHE);
        // delay(3000);
       // Avance(DIR_ARRIERE);
       servoM.write(Tab_Pos_Pied[POS_AVANT]);
       delay(1000);
      // servoD.write(Tab_Dir_Pied[Pos_Actuel_pied][DIR_AVANT]);
       servoD.write(50);
       delay(1000);
       servoD.write(5);
      }
    break;
/*    case TEST_SERVO:
      digitalWrite(ledoeil, 0);
    	if(digitalRead(!bouton_gauche)) {
    		//pos_m_0_180 ++;
    		//servoD.write(pos_m_0_180); 
        /*servoM.write(POS_HAUTE_A);
        delay(4000);
        Serial.print("position HAUTE_AVANT ");
        Serial.println(POS_HAUTE_A);
    		servoD.write(DIRECTION_AVANT);
        delay(4000);
        Serial.print("position DIRECTION_AVANT ");
        Serial.println(DIRECTION_AVANT);
        servoM.write(POS_HAUTE_B);
        delay(4000);
        Serial.print("position HAUTE_ARRIERE ");
        Serial.println(POS_HAUTE_B);
        
       // Avance(DIR_AVANT);
    	} 

    	if(digitalRead(!bouton_droit_oeil)) {
    		//pos_m_0_180 --;
    		//servoD.write(pos_m_0_180); 
    		/*servoM.write(POS_HAUTE_A);
        Serial.print("position HAUTE_AVANT ");
        Serial.println(POS_HAUTE_A);
        servoD.write(DIRECTION_AVANT);
        
     //   Avance(DIR_GAUCHE);
    	}
    	Serial.print("position ");
    	Serial.println(pos_m_0_180);
    	delay(100);
    break;

    case ATT_5_SEC:
    	for(int x=0;x<5;x++){
      		delay(100);
     		digitalWrite(ledoeil, 0);
      		delay(100);
      		digitalWrite(ledoeil, 1);
      	}
      
      	val_max=0;
     	Serial.println("Depart: cherche adv.");
       	servoM.write(POS_BASSE); pos_m_0_180=POS_BASSE;
    	delay(1000);
    	servoD.write(0);
    	delay(1000);
    	degree_adv = 0;
      	s_state_next = ON_ATTAQUE;
    break;

    case ATTAQUE_AVEUGLE:

    break;

    case CHERCHE_ADV:
    	digitalWrite(ledChap, 1);
    	servoM.write(POS_BASSE); pos_m_0_180=POS_BASSE;
    	delay(500);
    	servoD.write(0);
    	delay(1000);

      	for(int x=0;x<180;x++){
        	servoD.write(x);
        	tab_sensor0[x]=analogRead(capteurIRavant);
       	 	if(val_max<tab_sensor0[x]){
        		degree_adv = x;
          		val_max=tab_sensor0[x];
        	}
        	delay(50);
      	}
		Serial.print("Adversaire position: ");
		Serial.println(degree_adv);
		if(degree_adv-45>0) {
			degree_adv-=45;
		} else {
			degree_adv+=90-45;
		}
		Serial.print("Adversaire positioncompensait: ");
		Serial.println(degree_adv);
        if(val_max>100){
        	servoD.write(degree_adv);
        	delay(1000);
        	servoM.write(POS_HAUTE_A); pos_m_0_180=POS_HAUTE_A;
        	delay(1000);
        	s_state_next = ON_ATTAQUE;
        } else {
           val_max=0;
      		s_state_next = CHERCHE_ADV;
        }
    break;

    case ON_ATTAQUE:
      
    	if(pos_m_0_180==POS_HAUTE_A){
      		if((degree_adv+90)>180) {
        		servoD.write(degree_adv-90);delay(1000);
        	}
       		else {
        		servoD.write(degree_adv+90);delay(1000);
        	}
        	servoM.write(POS_HAUTE_B); pos_m_0_180=POS_HAUTE_B;
        	delay(1000);
      	} else {
        	servoD.write(degree_adv);
        	delay(1000);
        	servoM.write(POS_HAUTE_A); pos_m_0_180=POS_HAUTE_A;
        	delay(1000);
      	}
    	if(!digitalRead(bouton_gauche)) s_state_next = FIN;

    break;

    case FIN:
    servoD.write(degree_adv);
    servoM.write(POS_HAUTE_A);
    //val_max=0;
      break;
    default:
    ;*/
  }
  s_state = s_state_next;
}

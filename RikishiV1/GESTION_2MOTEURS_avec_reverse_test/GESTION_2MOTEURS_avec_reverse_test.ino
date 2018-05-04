/*
2 moteurs, tempo de demarrage et ralenti modif hervé et nico rajout temp par bouton
*/

/* Utilisation du capteur Ultrason HC-SR04 */
// définition des broches utilisées 
int trig = 13; 
int echo = 12; 
long lecture_echo; 
long cm;
int ddetecte = 40; // distance de detection

#define ledPin 3  /* anode led verte démarrage D3*/
#define enablePin 11  /* Moteur 1 Droit*/
#define enablePin1 6 /* Moteur 2*/ // MOTEUR_G_EN
#define in1Pin 10 /* Moteur 1*/
#define in2Pin 9  /* Moteur 1*/
#define in3Pin 4 /* Moteur 2/Gauche*/
#define in4Pin 7 /* Moteur 2/Gauche*/
#define potPin 0
#define START_BT 2

int switchPin = 8;  /* commute marche arriere*/
int speed = 0;

void attendre()
{
  digitalWrite(ledPin, LOW);
  delay(1000);
  digitalWrite(ledPin, HIGH);
}


void setMotor(int speed, boolean reverse)
{
  //Active les 2 moteurs
  analogWrite(enablePin, speed);
  analogWrite(enablePin1, speed);
  //reverse=0 => avance
  //!reverse=1 => recule
  digitalWrite(in1Pin, !reverse);
  digitalWrite(in2Pin, reverse);

  digitalWrite(in3Pin, !reverse);
  digitalWrite(in4Pin, reverse);
}

void setMotor1(int speed, boolean reverse)
{
  //Active les moteurs
  //analogWrite(enablePin, speed);
  analogWrite(enablePin1, speed);
  //reverse=0 => avance
  //!reverse=1 => recule
  digitalWrite(in1Pin, !reverse);
  digitalWrite(in2Pin, reverse);

  //digitalWrite(in3Pin, !reverse);
  //digitalWrite(in4Pin, reverse);
}
void setMotor2(int speed, boolean reverse)
{
  //Active le moteur1 uniquement
  analogWrite(enablePin, speed);
  //analogWrite(enablePin1, speed);
  //reverse=0 => avance
  //!reverse=1 => recule
  //digitalWrite(in1Pin, !reverse);
  //digitalWrite(in2Pin, reverse);

  digitalWrite(in3Pin, !reverse);
  digitalWrite(in4Pin, reverse);
}

void depart()
{
  boolean reverse;
  speed =255;
//recule pour faire tomber la pelle !
  reverse = 1;
  setMotor(speed, reverse);
  delay(1800);
  //stop
  reverse = 0;
  setMotor(0,reverse);
 digitalWrite(ledPin, LOW);
}

void avance()
{
  digitalWrite(trig, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(trig, LOW); 
  lecture_echo = pulseIn(echo, HIGH); 
  cm = lecture_echo / 58; 
 if (cm < ddetecte)
  {
  digitalWrite(ledPin, HIGH);
  //Serial.print("Objet detecte a : "); 
  //Serial.println(cm);
  boolean reverse;
  speed =255;
  reverse = 0;
  setMotor(speed, reverse);
  delay(100);
  }
  else 
  {
  //Serial.print("Distance : "); 
  //Serial.println(cm); 
  //Serial.println(ddetecte);
  digitalWrite(ledPin, LOW);
  boolean reverse;
  speed =150;
  reverse = 0;
  setMotor1(speed, reverse);
  setMotor2(speed, !reverse);
  //setMotor1(0,reverse);
 // setMotor2(0, reverse);
 }
}


void finplateau()
{
  boolean reverse;
  speed =255;
  reverse = 1;
  setMotor(speed, reverse);
  delay(1000);
  //stop
  reverse = 0;
  setMotor(0,reverse);  
}


void setup()
{
 //Serial.begin(9600);
  
  pinMode(trig, OUTPUT); 
  digitalWrite(trig, LOW); 
  pinMode(echo, INPUT); 
  
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(START_BT, INPUT_PULLUP);
  pinMode(enablePin1, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);
  //setup contact temp
  pinMode(ledPin, OUTPUT);
  
  while(digitalRead(START_BT)){
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(500);  
  }
    digitalWrite(ledPin, LOW);
    delay(4600);
    digitalWrite(ledPin, HIGH);
    
      //attendre();
      depart();
}

void loop()
{
 if (digitalRead(switchPin)== HIGH)
 { 
  finplateau(); 
 }
 else 
{
 avance();
 }
}

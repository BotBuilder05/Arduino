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
  

void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    
  pinMode(capteurIRavant, INPUT);
  pinMode(capteurIRdroit, INPUT);
  pinMode(capteurIRarriere, INPUT);
  pinMode(capteurIRgauche, INPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
      int valeurLue=0;

    valeurLue = analogRead(capteurIRavant);
     

    Serial.print("Avant: ");
    Serial.println(valeurLue);
    delay(500);

    valeurLue = analogRead(capteurIRarriere);

    Serial.print("Arriere: ");
    Serial.println(valeurLue);
    delay(500);

    valeurLue = analogRead(capteurIRdroit);
   
    Serial.print("Droit: ");
    Serial.println(valeurLue);
    delay(500);
    
    valeurLue = analogRead(capteurIRgauche);
      
    Serial.print("Gauche: ");
    Serial.println(valeurLue);
    delay(500);
}

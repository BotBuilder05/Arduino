/*********
  Xavier Charles
  Based on 
  Complete project details at http://randomnerdtutorials.com  
  and 
  https://electronza.com/gardena-water-timer-controller-arduino-uno/#prettyPhoto
  Using H-bride L9110 but just half of it as it was designed to control 2 motors.
  In order to change power signed I will use only  the Motor A pins
*********/

// Motor A
const int motorPinIA = 34; 
const int motorPinIB = 35; 

void setup() {
  // sets the pins as outputs:
  pinMode(motorPinIA, OUTPUT);
  pinMode(motorPinIB, OUTPUT);
  
  Serial.begin(115200);

  // testing
  Serial.print("Testing DC Motor...");
}


void forward(){
  // Move the DC motor forward at maximum speed
  Serial.println("Moving Forward");
  digitalWrite(motorPinIA, LOW);
  digitalWrite(motorPinIB, HIGH); 
  delay(2000);
}

void backward(){
  // Move DC motor backwards at maximum speed
  Serial.println("Moving Backwards");
  digitalWrite(motorPinIA, HIGH);
  digitalWrite(motorPinIB, LOW); 
  delay(2000);
}

void stop(){
  // Stop the DC motor
  Serial.println("Motor stopped");
  digitalWrite(motorPinIA, LOW);
  digitalWrite(motorPinIB, LOW);
  delay(1000);
}

void loop() {
  forward();

  stop();

  backward();

}

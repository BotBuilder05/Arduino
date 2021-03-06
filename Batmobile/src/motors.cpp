#include <Arduino.h>
#include "Batmobile.h"

#define PWM_MOTOR_1 0
#define PWM_MOTOR_2 1
#define PWM_FREQ 30000
#define PWM_RES 8

void setupMotors() {
	pinMode(MOTOR_1_A, OUTPUT);
	pinMode(MOTOR_1_B, OUTPUT);
	pinMode(MOTOR_1_EN, OUTPUT);
	pinMode(MOTOR_2_A, OUTPUT);
	pinMode(MOTOR_2_B, OUTPUT);
	pinMode(MOTOR_2_EN, OUTPUT);
	pinMode(RELAY, OUTPUT);

	ledcSetup(PWM_MOTOR_1, PWM_FREQ, PWM_RES);
	ledcAttachPin(MOTOR_1_EN, PWM_MOTOR_1);
	ledcSetup(PWM_MOTOR_2, PWM_FREQ, PWM_RES);
	ledcAttachPin(MOTOR_2_EN, PWM_MOTOR_2);
}

void activeBoost() {
	digitalWrite(RELAY, HIGH);
}

void desactiveBoost() {
	digitalWrite(RELAY, LOW);
}

void move(uint8_t sens, uint8_t speed) {
	switch (sens) {
		case FORWARD:
			digitalWrite(MOTOR_1_A, HIGH);
			digitalWrite(MOTOR_1_B, LOW);
			digitalWrite(MOTOR_2_A, HIGH);
			digitalWrite(MOTOR_2_B, LOW);
			break;

		case BACKWARD:
			digitalWrite(MOTOR_1_A, LOW);
			digitalWrite(MOTOR_1_B, HIGH);
			digitalWrite(MOTOR_2_A, LOW);
			digitalWrite(MOTOR_2_B, HIGH);
			break;

		case RIGHT:
			digitalWrite(MOTOR_1_A, HIGH);
			digitalWrite(MOTOR_1_B, LOW);
			digitalWrite(MOTOR_2_A, LOW);
			digitalWrite(MOTOR_2_B, HIGH);
			break;

		case LEFT:
			digitalWrite(MOTOR_1_A, LOW);
			digitalWrite(MOTOR_1_B, HIGH);
			digitalWrite(MOTOR_2_A, HIGH);
			digitalWrite(MOTOR_2_B, LOW);
			break;
	}

	setSpeed(speed, speed);
}

void setSpeed(uint8_t left, uint8_t right) {
	/*digitalWrite(MOTOR_1_EN, left);
	digitalWrite(MOTOR_2_EN, right);*/
	ledcWrite(PWM_MOTOR_1, left);
	ledcWrite(PWM_MOTOR_2, right);
}

void stop() {
	digitalWrite(MOTOR_1_A, LOW);
	digitalWrite(MOTOR_1_B, LOW);
	digitalWrite(MOTOR_2_A, LOW);
	digitalWrite(MOTOR_2_B, LOW);
	setSpeed(255,255);
}

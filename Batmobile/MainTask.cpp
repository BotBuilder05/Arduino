#include "FlashMcQueen.h"
#include <Arduino.h>

uint8_t next_state;
hw_timer_t* timer = NULL;
Settings::Setting_t config;
xTaskHandle ParseTaskH;

SemaphoreHandle_t parseSem;
//flags
int counter = 0;
uint8_t ennemy_front = 0,
		ennemy_near = 0,
		ennemy_front_left = 0,
		ennemy_front_right = 0,
		ennemy_left = 0,
		ennemy_right = 0,
		white_line = 0;

void ParseTask(void *pvParams)
{
	for (;;) {
		if (xSemaphoreTake(parseSem, portMAX_DELAY)) {
			SensorRead_t val = readAll();
			Serial.printf("%dcm|%dcm\n", val.laser[1], val.laser[0]);
			/* white line test */
			if (val.color1 > config.color_black_limit || val.color2 > config.color_black_limit)
				white_line = 1;
			else
				white_line = 0;

			/* ennemy position */
			if (val.laser[0] + val.laser[1] < config.detect_distance * 2) {
				ennemy_front = 1;
				if (val.laser[0] + val.laser[1] < config.boost_distance * 2)
					ennemy_near = 1;
				else
					ennemy_near = 0;
			} else {
				ennemy_front = 0;
				ennemy_near = 0;
				if (val.laser[0] < config.detect_distance)
					ennemy_front_right = 1;
				else if (val.laser[1] < config.detect_distance)
					ennemy_front_left = 1;
				else {
					ennemy_front_left = 0;
					ennemy_front_right = 0;
				}
			}

			//Serial.printf("%d|%d|%d\n", white_line, ennemy_front, ennemy_near);
		}
	}
}

void IRAM_ATTR timedRead(void)
{
	BaseType_t wake = pdTRUE;
	xSemaphoreGiveFromISR(parseSem, &wake);
}

void IRAM_ATTR stopMainTask(void) {
	next_state = END;
}

void MainTask(void* pvParams)
{
	uint8_t current_state;
	current_state = next_state = INIT;

	char log[100];

	parseSem = xSemaphoreCreateBinary();
	xTaskCreatePinnedToCore(
		ParseTask,
		"ParseTask",
		5000,
		NULL,
		3,
		&ParseTaskH,
		XCORE_2
	);

	Serial.println("Creating timer...");

	timer = timerBegin(0, 80, true);
	timerAttachInterrupt(timer, timedRead, true);
	timerAlarmWrite(timer, TIMER_INTERRUPT, true);
	timerAlarmEnable(timer);
	//timerStop(timer);

	attachInterrupt(MICROSTART_IN, stopMainTask, FALLING);
	vTaskDelay(100);

	while (true) {
		switch (current_state) {
			case INIT:
				digitalWrite(MICROSTART_EN, HIGH);
				next_state = READY;
				break;

			case READY:
				if (digitalRead(MICROSTART_IN) == HIGH) {
					//digitalWrite(MICROSTART_EN, LOW);
					next_state = SEARCH;
				}

				break;

			case SEARCH:
				//TASK_LOG("Searching...");
				move(RIGHT, 210);
				if (ennemy_front)
					next_state = ATTACK;
				else if (white_line)
					next_state = ESCAPE;
				break;

			case ATTACK:
				move(FORWARD);
				if (ennemy_near)
					next_state = ATTACK_BOOST;
				else if (white_line)
					next_state = ESCAPE;
				else if (ennemy_front == 0)
					next_state = SEARCH;
				
				break;

			case ATTACK_BOOST:
				activeBoost();
				if (ennemy_near == 0) {
					if (ennemy_front_left)
						setSpeed(230, 255);
					else if (ennemy_front_right)
						setSpeed(255, 230);
					else {
						desactiveBoost();
						next_state = ATTACK;
					}
				}
				break;

			case ESCAPE:
				if (counter++ < 100 && white_line && !ennemy_front)
					move(BACKWARD, 200);
				else {
					counter = 0;
					next_state = SEARCH;
				}
				break;

			case END:
				//stop motor
				timerStop(timer);
				vTaskDelete(ParseTaskH);
				TASK_LOG("MainTask : End state");
				digitalWrite(MICROSTART_EN, LOW);
				setSpeed(0, 0);
				desactiveBoost();
				xSemaphoreGive(((TaskParam_t*)pvParams)->sem);
				vTaskDelay(1000);
				break;
		}

		current_state = next_state;
		vTaskDelay(5);
	}
	return;
}
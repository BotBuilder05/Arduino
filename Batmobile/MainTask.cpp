#include "FlashMcQueen.h"
#include <Arduino.h>

uint8_t next_state;
hw_timer_t* timer = NULL;
ArduinoJson6141_0000010::JsonDocument config;
xTaskHandle ParseTaskH;

SemaphoreHandle_t parseSem;
//flags
int counter = 0;
uint8_t ennemy_front = 0,
		ennemy_near = 0,
		ennemy_front_left = 0,
		ennemy_front_right = 0,
		ennemy_left = 0,
		ennemy_right = 0;

void ParseTask(void *pvParams)
{
	for (;;) {
		if (xSemaphoreTake(parseSem, portMAX_DELAY)) {
			SensorRead_t val = readAll();
			Serial.printf("%d|%d|%d|%d cm\n", val.laser[0], val.laser[1], val.laser[2], val.laser[3]);
			/* white line test */
			if (val.color1 > config["color_black_limit"] || val.color2 > config["color_black_limit"]) {
				next_state = ESCAPE;
			}

			/* ennemy position */
			if (val.laser[0] + val.laser[1] < config["detect_distance"] * 2) {
				ennemy_front = 1;
				if (val.laser[0] + val.laser[1] < config["boost_distance"] * 2)
					ennemy_near = 1;
				else
					ennemy_near = 0;
			}
			else {
				ennemy_front = 0;
				ennemy_near = 0;
				if (val.laser[0] < config["detect_distance"])
					ennemy_front_right = 1;
				else if (val.laser[1] < config["detect_distance"])
					ennemy_front_left = 1;
				else {
					ennemy_front_left = 0;
					ennemy_front_right = 0;
				}
			}
			if (val.laser[2] < config["detect_distance"]) {
				ennemy_left = 1;
			}
			else {
				ennemy_left = 1;
			}
			if (val.laser[3] < config["detect_distance"]) {
				ennemy_right = 1;
			}
			else {
				ennemy_right = 1;
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
	char log[LOG_SIZE], cmd[CMD_SIZE];

	config = *((TaskParam_t*)pvParams)->set;

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

	if (config["mode"] == SETTING_MODE_TEST) {
		current_state = next_state = DEBUG;
		timerStop(timer);
	}

	vTaskDelay(100);

	while (true) {
		log[0] = '\0';
		switch (current_state) {
			case INIT:
				if (config["start_mode"] == SETTING_START_MICROSTART) {
					attachInterrupt(MICROSTART_IN, stopMainTask, FALLING);
					digitalWrite(MICROSTART_EN, HIGH);
				}
				next_state = READY;
				break;

			case READY:
				if (config["start_mode"] == SETTING_START_MICROSTART) {
					//if (digitalRead(MICROSTART_IN) == HIGH) {
						next_state = START_SEQ;
					//}
				}
				else if (config["start_mode"] == SETTING_START_5SEC) {
					if (xQueueReceive(
						((TaskParam_t*)pvParams)->cmd,
						cmd,
						0
						)) {
						if (strcmp(cmd, CMD_RUN) == 0) {
							/* 5000ms delay */
							next_state = START_SEQ;
							vTaskDelay(4900 / portTICK_PERIOD_MS);
						}
					}
				}

				break;

			case START_SEQ:
				move(BACKWARD);
				vTaskDelay(300 / portTICK_PERIOD_MS);
				next_state = ATTACK_BOOST;
				if (ennemy_left) {
					move(LEFT);
					vTaskDelay(100 / portTICK_PERIOD_MS);
				}
				else if (ennemy_right) {
					move(RIGHT);
					vTaskDelay(100 / portTICK_PERIOD_MS);
				} else
					next_state = SEARCH;
				break;

			case SEARCH:
				//TASK_LOG("Searching...");
				move(RIGHT, 210);
				if (ennemy_front)
					next_state = ATTACK;
				break;

			case ATTACK:
				move(FORWARD);
				if (ennemy_near)
					next_state = ATTACK_BOOST;
				else if (ennemy_front == 0)
					next_state = SEARCH;

				break;

			case ATTACK_BOOST:
				activeBoost();
				counter++;
				if (ennemy_near == 0) {
					if (ennemy_front_left)
						setSpeed(230, 255);
					else if (ennemy_front_right)
						setSpeed(255, 230);
					else {
						desactiveBoost();
						counter = 0;
						next_state = ATTACK;
					}
				}
				else if (counter > config["boost_count_max"]) {
					desactiveBoost();
					counter = 0;
					next_state = ESCAPE;
				}
				break;

			case ESCAPE:
				if (counter++ < config["escape_count_max"] && !ennemy_front)
					move(BACKWARD, 200);
				else {
					counter = 0;
					if (ennemy_front)
						next_state = ATTACK;
					else
						next_state = SEARCH;
				}
				break;

			case END:
				timerStop(timer);
				vTaskDelete(ParseTaskH);
				TASK_LOG("MainTask : End state");
				digitalWrite(MICROSTART_EN, LOW);
				/* stop the bot */
				setSpeed(0, 0);
				desactiveBoost();

				/* same as return */
				xSemaphoreGive(((TaskParam_t*)pvParams)->sem);
				vTaskDelay(1000);
				break;

			case DEBUG:
				vTaskDelay(100);
				SensorRead_t val = readAll();
				sprintf(log, "Laser: %d|%d|%d|%d Color: %d|%d\n", val.laser[0], val.laser[1], val.laser[2], val.laser[3], val.color1, val.color2);
				break;
		}

		current_state = next_state;
		if(strlen(log)>0) TASK_LOG(log);
	}
	return;
}
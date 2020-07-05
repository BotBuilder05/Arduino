#include <Arduino.h>
#include <WifiHandler.hpp>
#include <Settings.hpp>
#include <Sensors.hpp>
#include "Batmobile.h"

#define wait(ms) timeToWait = ms; saved_state = current_state; timeCounter=millis(); next_state = WAIT

uint8_t next_state, urgent_state = 0;
hw_timer_t* timer = NULL;
xTaskHandle ParseTaskH;

SensorRead_t val;
uint8_t detect_distance, boost_distance;

SemaphoreHandle_t parseSem;
//flags
long timeCounter = 0, timeToWait = 0;
uint16_t counter = 0;
uint8_t ennemy_start_pos = RIGHT,
		ennemy_front = 0,
		ennemy_near = 0,
		ennemy_front_left = 0,
		ennemy_front_right = 0,
		ennemy_left = 0,
		ennemy_left_near = 0,
		ennemy_right_near = 0,
		ennemy_right = 0,
		white_line_left = 0,
		white_line_right = 0;

void ParseTask(void *pvParams)
{
	for (;;) {
		if (xSemaphoreTake(parseSem, portMAX_DELAY)) {
			val = Sensors::readAll();
			//Serial.printf("%d|%d|%d|%d cm\n", val.laser[0], val.laser[1], val.laser[2], val.laser[3]);
			/* white line test */
			white_line_left = val.color1 > 100;
			white_line_right = val.color2 > 100;
			if (white_line_left && white_line_right)
				urgent_state = ESCAPE;
			else if (white_line_left)
				urgent_state = ESCAPE_TO_RIGHT;
			else if (white_line_right)
				urgent_state = ESCAPE_TO_LEFT;
			else
				urgent_state = 0;

			/* ennemy position */
			if (val.laser[0] < detect_distance && val.laser[1] < detect_distance) {
				ennemy_front = 1;
				ennemy_near = (val.laser[0] < boost_distance && val.laser[1] < boost_distance);
			} else {
				ennemy_front = 0;
				ennemy_near = 0;
				ennemy_front_right = val.laser[0] < detect_distance;
				ennemy_front_left = val.laser[1] < detect_distance;
			}

			ennemy_left = val.laser[2] < detect_distance;
			ennemy_left_near = val.laser[2] < boost_distance;
			ennemy_right = val.laser[3] < detect_distance;
			ennemy_right_near = val.laser[3] < boost_distance;

			//Serial.printf("%d|%d\n", white_line_left, white_line_right);
		}
	}
}

void IRAM_ATTR timedRead(void)
{
	BaseType_t wake = pdTRUE;
	xSemaphoreGiveFromISR(parseSem, &wake);
}

void IRAM_ATTR stopMainTask(void)
{
	next_state = END;
}

void MainTask(void* pvParams)
{
	uint8_t current_state, saved_state = 0;
	current_state = next_state = INIT;
	char cmd[CMD_SIZE];

	detect_distance = Settings::set["detect_distance"];
	boost_distance = Settings::set["boost_distance"];

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

	if (Settings::set["mode"].as<String>().equals(SETTING_MODE_TEST)) {
		current_state = next_state = DEBUG;
		timerStop(timer);
	}

	while (true) {
		switch (current_state) {
			case INIT:
				if (Settings::set["start_mode"].as<String>().equals(SETTING_START_MICROSTART)) {
					WifiHandler::_client.println("> Microstart started");
					attachInterrupt(MICROSTART_IN, stopMainTask, FALLING);
					digitalWrite(MICROSTART_EN, HIGH);
				}
				next_state = READY;
				break;

			case READY:
				if (Settings::set["start_mode"].as<String>().equals(SETTING_START_MICROSTART)) {
					if (digitalRead(MICROSTART_IN) == HIGH) {
						next_state = START_SEQ;
					}
				}
				else if (Settings::set["start_mode"].as<String>().equals(SETTING_START_5SEC)) {
					if (xQueueReceive(
						((TaskParam_t*)pvParams)->cmd,
						cmd,
						0
						)) {
						if (strcmp(cmd, CMD_RUN) == 0) {
							WifiHandler::_client.println("=> start in 5 sec");
							/* 5000ms delay */
							next_state = START_SEQ;
							delay(4800);
						}
					}
				}

				break;

			case START_SEQ:
				ennemy_start_pos = ennemy_left ? LEFT : RIGHT;
				move(BACKWARD);
				delay(100);
				move(FORWARD);
				delay(40);
				move(BACKWARD);

				if (ennemy_left)
					ennemy_start_pos = LEFT;
				else if(ennemy_right)
					ennemy_start_pos = RIGHT;

				if (Settings::set["strategy"].as<String>().equals(SETTING_STRATEGY_BASIC)) {
					delay(100);
					move(ennemy_start_pos);
					delay(100);
					setSpeed(0, 0);
					delay(100);
					next_state = SEARCH;
				} 
				else if (Settings::set["strategy"].as<String>().equals(SETTING_STRATEGY_DIFF_START)) {
					move(ennemy_start_pos);
					delay(200);
					move(FORWARD);
					delay(50);
					next_state = ATTACK;
				}
				else if (Settings::set["strategy"].as<String>().equals(SETTING_STRATEGY_DIFF_START2)) {
					move(ennemy_start_pos);
					delay(250);
					move(BACKWARD);
					delay(70);
					next_state = SEARCH;
				}
				else {
					next_state = SEARCH;
				}
				timeCounter = millis();
				break;

			case SEARCH:
				//WifiHandler::_client.println("=> Searching");
				//setSpeed(0,0);
				move(ennemy_start_pos, 225);
				//setSpeed(ennemy_start_pos == LEFT ? 230 : 150, ennemy_start_pos == LEFT ? 230 : 150);

				if (millis() - timeCounter > 200)
					stop();

				wait(300);
				break;

			case FOLLOW_LEFT:
				WifiHandler::_client.println("=> Follow left");
				move(FORWARD);
				setSpeed(200, 230);
				if (ennemy_front)
					next_state = ATTACK;
				else if (!ennemy_front_left)
					next_state = SEARCH;
				wait(10);
				break;

			case FOLLOW_RIGHT:
				WifiHandler::_client.println("=> Follow right");
				move(FORWARD);
				setSpeed(230, 200);
				if (ennemy_front)
					next_state = ATTACK;
				else if (!ennemy_front_right)
					next_state = SEARCH;
				wait(10);
				break;

			case ATTACK:
				WifiHandler::_client.println("=> Attack");
				desactiveBoost();
				move(FORWARD);
				if (ennemy_near)
					next_state = ATTACK_BOOST;
				else if (!ennemy_front) {
					if (ennemy_front_left)
						next_state = FOLLOW_LEFT;
					else if (ennemy_front_right)
						next_state = FOLLOW_RIGHT;
					else
						next_state = SEARCH;
				}
				break;

			case ATTACK_BOOST:
				WifiHandler::_client.println("=> Rush");
				move(FORWARD);
				activeBoost();
				if (ennemy_near == 0) {
					desactiveBoost();
					counter = 0;
					next_state = ennemy_front ? ATTACK : SEARCH;
				}
				break;

			case ESCAPE:
				WifiHandler::_client.println("=> White line detected");
				move(BACKWARD);
				delay(Settings::set["escape_count_max"]);
				//if (pdMS_TO_TICKS(++counter) > Settings::set["escape_count_max"]) {
					counter = 0;
					next_state = ennemy_front ? ATTACK : SEARCH;
				//}
				break;

			case ESCAPE_TO_LEFT:
				move(LEFT, 230);
				delay(Settings::set["escape_count_max"]);
				next_state = ennemy_front ? ATTACK : SEARCH;
				break;

			case ESCAPE_TO_RIGHT:
				move(RIGHT, 230);
				delay(Settings::set["escape_count_max"]);
				next_state = ennemy_front ? ATTACK : SEARCH;
				break;

			case TURN_RIGHT:
				if(ennemy_right_near) {
					move(BACKWARD);
					setSpeed(240, 255);
					wait(1000);
				}
				move(RIGHT);
				wait(200);
				saved_state = ATTACK;
				break;
			
			case TURN_LEFT:
				if(ennemy_left_near) {
					move(BACKWARD);
					setSpeed(255, 240);
					wait(1000);
				}
				move(LEFT);
				wait(200);
				saved_state = ATTACK;
				break;

			case WAIT:
				if (ennemy_front || ennemy_front_left || ennemy_front_right) {
					next_state = ATTACK;
				} else if (ennemy_left && saved_state != TURN_LEFT) {
					next_state = TURN_LEFT;
				} else if (ennemy_right && saved_state != TURN_RIGHT) {
					next_state = TURN_RIGHT;
				}
				else if(millis() - timeCounter >= timeToWait) {
					timeCounter = millis();
					next_state = saved_state;
				}
				break;

			case END:
				timerStop(timer);
				vTaskDelete(ParseTaskH);
				digitalWrite(MICROSTART_EN, LOW);
				/* stop the bot */
				setSpeed(0, 0);
				desactiveBoost();

				WifiHandler::_client.println("> Main task ended");

				/* same as return */
				xSemaphoreGive(((TaskParam_t*)pvParams)->sem);
				vTaskDelay(1000);
				break;

			case DEBUG_MOTOR:
				for (uint8_t i = 150; i < 255; i++) {
					move(FORWARD, i);
					if (i == 253)
						activeBoost();
					delay(100);
				}
				desactiveBoost();
				break;

			case DEBUG:
				vTaskDelay(100);
				SensorRead_t val = Sensors::readAll();
				WifiHandler::_client.printf("Laser: %d|%d|%d|%d Color: %d|%d\n", val.laser[0], val.laser[1], val.laser[2], val.laser[3], val.color1, val.color2);
				break;
		}

		current_state = urgent_state ? urgent_state : next_state;
		vTaskDelay(1);
	}
	return;
}

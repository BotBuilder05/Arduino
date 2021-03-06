#include <Wire.h>
#include <Settings.hpp>
#include "drivers/VL53L0X.h"
#include "drivers/veml6040.h"
#include <Settings.hpp>
#include <Sensors.hpp>
#include <WifiHandler.hpp>
#include "Batmobile.h"

VL53L0X lasers[NB_LASER];
VEML6040 color1(&Wire), color2(&Wire1);
uint8_t lasers_pins[NB_LASER] = { LASER_1_XSHUT, LASER_2_XSHUT, LASER_3_XSHUT, LASER_4_XSHUT };
SensorRead_t reads;
uint16_t cl1_cal, cl2_cal;

void Sensors::init()
{
	Serial.println("[+] Init I2C");
	Wire.begin(I2C_SDA2, I2C_SCL2);
	Wire1.begin(I2C_SDA1, I2C_SCL1);

	for (uint8_t i = 0; i < NB_LASER; i++) {
		pinMode(lasers_pins[i], OUTPUT);
		digitalWrite(lasers_pins[i], LOW);
	}

	for (uint8_t i = 0; i < NB_LASER; i++) {
		digitalWrite(lasers_pins[i], HIGH);
		vTaskDelay(100);
		lasers[i].init();
		vTaskDelay(100);
		lasers[i].setTimeout(500);
		lasers[i].setAddress(LASER_BASE_ADDR + i);
		lasers[i].startContinuous((TIMER_INTERRUPT / 1000) - 5);
	}

	color1.begin();
	color1.setConfiguration(VEML6040_IT_40MS + VEML6040_AF_AUTO + VEML6040_SD_ENABLE);
	color2.begin();
	color2.setConfiguration(VEML6040_IT_40MS + VEML6040_AF_AUTO + VEML6040_SD_ENABLE);

	cl1_cal = Settings::set["color1_calibration"].as<uint16_t>();
	cl2_cal = Settings::set["color2_calibration"].as<uint16_t>();
}

void Sensors::calibrate()
{
	int avg1 = 0, avg2 = 0;
	for(uint8_t i = 0; i < 50; i++) {
		avg1 += color1.getWhite();
		avg2 += color2.getWhite();
	}
	Settings::set["color1_calibration"] = cl1_cal = avg1 / 50;
	Settings::set["color2_calibration"] = cl2_cal = avg2 / 50;
	WifiHandler::_client.printf("[+] Color 1 calibrated to %d and Color 2 to %d\n", avg1/50, avg2/50);
	Settings::save();
}

SensorRead_t Sensors::readAll()
{

	for (uint8_t i = 0; i < NB_LASER; i++)
		reads.laser[i] = lasers[i].readRangeContinuousMillimeters() / 10;

	reads.color1 = color1.getWhite() - cl1_cal;
	reads.color2 = color2.getWhite() - cl2_cal;

	return reads;
}

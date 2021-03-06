#ifndef BATMOBILE_H
#define BATMOBILE_H

#include <stdint.h>
#include <driver/i2c.h>

#define VERSION "0.1.0"

/* ##### RTOS ##### */
#define XCORE_1 (BaseType_t)0
#define XCORE_2 (BaseType_t)1

#define LOG_SIZE 1000 //log string size
#define LOG_MAX 5 //max logs in queue
#define CMD_SIZE 1000
#define CMD_MAX 3
typedef uint8_t Cmd_t;

#define TIMER_INTERRUPT 50000 //50ms

#define LOG(s) xQueueSend(Global_log_queue, s, 0)
#define TASK_LOG(s) xQueueSend(((TaskParam_t*)pvParams)->log, s, 0);
/* ################ */

/* #### Commands ##### */
#define CMD_UPDATE "UPD"
#define CMD_STOP "STP"
#define CMD_START "SRT"
#define CMD_SET "SET"
#define CMD_RESET "RST"
#define CMD_RUN "RUN"
#define CMD_GET "GET"
#define CMD_SETGET_JSON "JSON"
#define CMD_SET_PARAM "PARAM"

#define CMD_CALIBRATE "CLB"

#define CMD_SET_MODE "MODE"
#define CMD_MODE_AUTO "1"
#define CMD_MODE_MANUAL "2"
#define CMD_MODE_TEST "3"
/* #################### */


/* ###### Wifi infos ####### */
#define AP_SSID "Batmobile"
#define AP_PASS "BAT_INSANEPwd"

#define LOCAL_IP (uint8_t*)"192.168.0.1"
#define GATEWAY_IP (uint8_t*)"192.168.1.1"
#define SUBNET_IP (uint8_t*)"255.255.255.0"

#define SERVER_PORT 666
/* ######################### */


/* ###### Pins Infos ####### */
#define I2C_SDA1 22
#define I2C_SCL1 21
#define I2C_SDA2 2
#define I2C_SCL2 15

#define NB_LASER 4
#define LASER_1_XSHUT 33
#define LASER_2_XSHUT 25
#define LASER_3_XSHUT 26
#define LASER_4_XSHUT 13
#define LASER_BASE_ADDR 0x30

#define MICROSTART_EN 27
#define MICROSTART_IN 35

#define BLUE_LED 14

#define RELAY 32
#define MOTOR_1_EN 17
#define MOTOR_1_A 16
#define MOTOR_1_B 4
#define MOTOR_2_EN 5
#define MOTOR_2_A 19
#define MOTOR_2_B 18
/* ######################### */

/* ##### States ##### */
#define INIT 1
#define READY 2
#define SEARCH 3
#define ATTACK 4
#define END 5
#define ATTACK_BOOST 6
#define ESCAPE 9
#define START_SEQ 10
#define DEBUG 11
#define FOLLOW_LEFT 12
#define FOLLOW_RIGHT 13
#define DEBUG_MOTOR 14
#define ESCAPE_TO_LEFT 15
#define ESCAPE_TO_RIGHT 16
#define WAIT 17
#define TURN_RIGHT 18
#define TURN_LEFT 19
/* ################## */

/* ##### MOVE SENS ##### */
#define FORWARD 2
#define BACKWARD 1
#define RIGHT 3
#define LEFT 4
/* ##################### */

typedef struct {
	xQueueHandle log;
	xQueueHandle cmd;
	SemaphoreHandle_t sem;
} TaskParam_t;

/* ###### Proto function ###### */

//motors
void setupMotors(void);
void move(uint8_t sens, uint8_t speed = 255);
void setSpeed(uint8_t speed, uint8_t speed2);
void activeBoost(void);
void desactiveBoost(void);
void stop();


//interrupt
void stopMainTask(void);

/* Tasks */
void IdleLoop(void*);
void MainTask(void*);
/* ############################ */

#endif

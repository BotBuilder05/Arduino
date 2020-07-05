#ifndef SENSORS_H
#define SENSORS_H
#include <Batmobile.h>

typedef struct {
	uint16_t laser[NB_LASER];
	int16_t color1;
	int16_t color2;
} SensorRead_t;

class Sensors {

    public:
        Sensors() = delete;

        static void calibrate(void);
        static SensorRead_t IRAM_ATTR readAll(void);
        static void init(void);

};

#endif

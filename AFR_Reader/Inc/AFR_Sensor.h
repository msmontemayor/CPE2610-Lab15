/*
 * AFR_Sensor.h
 *
 *  Created on: May 5, 2026
 *      Author: montemayorm
 */

#ifndef AFR_SENSOR_H_
#define AFR_SENSOR_H_

#include <stdint.h>

typedef struct{
	uint32_t narrowband;
	uint32_t wideband;
}READINGS;

typedef enum{
	narrowband,
	wideband
}channel_e;

READINGS sensor_read();
#endif /* AFR_SENSOR_H_ */

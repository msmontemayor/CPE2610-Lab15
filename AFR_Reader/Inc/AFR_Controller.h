/*
 * AFR_Controller.h
 *
 *  Created on: May 5, 2026
 *      Author: montemayorm
 */

#ifndef AFR_CONTROLLER_H_
#define AFR_CONTROLLER_H_

#include <stdint.h>

// lambda table from the bosch datasheet
#define TABLE_SIZE 24
static const float ip_table[TABLE_SIZE] = {
    -2.000f, -1.602f, -1.243f, -0.927f, -0.800f, -0.652f, -0.405f, -0.183f,
    -0.106f, -0.040f,  0.000f,  0.015f,  0.097f,  0.193f,  0.250f,  0.329f,
     0.671f,  0.938f,  1.150f,  1.385f,  1.700f,  2.000f,  2.150f,  2.250f
};

static const float lambda_table[TABLE_SIZE] = {
     0.650f,  0.700f,  0.750f,  0.800f,  0.822f,  0.850f,  0.900f,  0.950f,
     0.970f,  0.990f,  1.003f,  1.010f,  1.050f,  1.100f,  1.132f,  1.179f,
     1.429f,  1.701f,  1.990f,  2.434f,  3.413f,  5.391f,  7.506f, 10.119f
};

// divider values for the gpio protection
#define R_DIV_UP       2200.0f
#define R_DIV_LOW      3300.0f
static const float DIVIDER_RATIO = (R_DIV_UP + R_DIV_LOW) / R_DIV_LOW;


#define TARGET_NERNST  0.450	// target voltage of the nernst cell
#define TARGET_IP_RES  0.350	// expected voltage jump when pulse applied
#define DEADBAND       0.005	// deadband because resistors are mostly 5%
#define VREF 3.3
#define ADC_RES        4095.0


void AFR_controller_init();
void AFR_pump_control();
void AFR_control_heater();
float get_nernst_resistance();
float get_lambda();
uint32_t read_ADC(uint32_t channel);
float ADC_To_Volt(uint32_t raw_adc);

#endif /* AFR_CONTROLLER_H_ */

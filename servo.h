/*
 * servo.h
 *
 *  Created on: Oct 17, 2018
 *      Author: ecpower
 */

#ifndef SERVO_H_
#define SERVO_H_

#include <inc/tm4c123gh6pm.h>
#include <stdint.h>
#include <lcd.h>

void servo_init();

void servo_move(int degree);

//void servo_calibrate(void);

#endif /* SERVO_H_ */

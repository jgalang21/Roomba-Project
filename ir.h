/*
*
*   ir.h
*
*/

#ifndef IR_H_
#define IR_H_

#include "Timer.h"
#include <stdio.h>
#include <stdint.h>
#include <inc/tm4c123gh6pm.h>

void ir_init(void);

int ir_read(void);

float ir_getDistance();

volatile unsigned isr_int_value;

#endif /* IR_H_ */

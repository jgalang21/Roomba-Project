/*
 * movement.h
 *
 * Contains functions to move robot a specific distance
 * and turn left or right a certain number of degrees
 *
 * @author Erin Power and Alejandro Buentello
 * @date 09/05/2018
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "Timer.h"
#include "lcd.h"
#include "scan.h"
#include <inc/tm4c123gh6pm.h>
#include "open_interface.h"

#define M_PI 3.14159265358979323846


bool statusCheck(oi_t*sensor);
//Move forward
void move_forward(oi_t*sensor, int dist);

//Move Back
void move_back(oi_t*sensor, int dist);

//Turn clockwise
void turn(oi_t*sensor, int degrees);
void load_songs();


#endif /* MOVEMENT_H_ */

/*
 * movement.c
 *
 * Created on: Sept 5, 2018
 *      Authors: Erin Power and Alejandro Buentello
 *
 *      THESE SETTINGS ARE FOR BOT 10 (12/1)
 */

// Move forward
#include "movement.h"
#include "scan.h"
#include "uart.h"
#include "ir.h"
#include "ping.h"
#define IMERPIAL_MARCH 1

bool statusCheck(oi_t*sensor_data)
{
    int boundup = 2800; //used for color detection
    int bounddown = 2600; //minumum threshold to detect bounds
    int dist_back = 50; //move back 10 cm

    if (sensor_data->cliffFrontLeft || sensor_data->cliffLeft)
    { //If the left sensor is triggered by a obstacle or pit
        uart_sendStr("Left sensor reached a cliff \r\n");
        move_back(sensor_data, dist_back); //Move back given distance

        return true;
    }
    else if (sensor_data->bumpLeft) //bump an object on the left
    {
        uart_sendStr("Bumped on left\r\n");
        move_back(sensor_data, dist_back); //Move back given distance

        return true;
    }
    else if (sensor_data->cliffFrontRight || sensor_data->cliffRight)
    {  //If the right sensor is triggered by obstacle or pit
        uart_sendStr("Right sensor reached a cliff \r\n");
        move_back(sensor_data, dist_back); //Move back given distance

        return true;
    }
    else if (sensor_data->bumpRight) //bump an object on the right
    {
        uart_sendStr("Bumped object on right \r\n");
        move_back(sensor_data, dist_back); //Move back given distance
        return true;

    }
    else if (sensor_data->cliffFrontRightSignal > boundup //color detection for the ORANGE paper
    && sensor_data->cliffFrontLeftSignal > boundup)
    {
        load_songs();
        uart_sendStr("In the zone. youR PapI Is ProUD of YoU!!!! \r\n");

    }
    else if (sensor_data->cliffFrontLeftSignal > bounddown //if both sensors hit the white tape
            && sensor_data->cliffFrontLeftSignal < boundup
            && sensor_data->cliffFrontRightSignal > bounddown
            && sensor_data->cliffFrontRightSignal < boundup)
    { //if border is detected on left side
        uart_sendStr("Out of bounds detected on front side, redirecting... \r\n");
        return true;
    }
    else if (sensor_data->cliffFrontLeftSignal > bounddown //for white tape
            && sensor_data->cliffFrontLeftSignal < boundup)
    { //if border is detected on left side
        uart_sendStr("Out of bounds on left side, redirecting... \r\n");
        return true;
    }
    else if (sensor_data->cliffFrontRightSignal > bounddown //for white tape
            && sensor_data->cliffFrontRightSignal < boundup)
    { //if border is detected on right side
        uart_sendStr("Out of bounds on right side, redirecting... \r\n");
        //turn(sensor_data, 90); //Turn to left (counterclockwise)
        return true;
    }


    return false;
}

void move_forward(oi_t*sensor_data, int dist)
{
    int sum = 0;
    float ir_dist = ir_getDistance();
    float ping_dist = ping_getDistance();

    while (sum < dist)
    { //Moves until distance is reached
      //get distance
        ir_dist = ir_getDistance();
        ping_dist = ping_getDistance();
        if ((ir_dist < 10) || (ping_dist < 10))
        { // if sensors detect anything 15 cm directly infront

            char UARTsend[100];
            sprintf(UARTsend, "Object in front of robot: %4.4f cm", ping_dist); //convert float to char order is degrees ir ping
            uart_sendStr(UARTsend);

            sum = dist;
        }
        else
        {
            oi_setWheels(280, 250); //for bot 10
            oi_update(sensor_data);
            bool temp = statusCheck(sensor_data);
            if (temp == false)
                sum += abs(sensor_data->distance);
            else
            {
                sum = dist;
                move_back(sensor_data, 50);
            }
        }
    }
    oi_setWheels(0, 0); //stops movement
}

void move_back(oi_t*sensor_data, int dist)
{ //Same as above but moves backwards
    int sum = 0;

    while (sum < dist)
    { //Moves in millimeters
        oi_setWheels(-280, -250);
        oi_update(sensor_data);
        sum += abs(sensor_data->distance);
    }
    oi_setWheels(0, 0); //stops movement
}

//Turn clockwise
void turn(oi_t*sensor_data, int degrees)
{
    int turn = 0;

    if (degrees < 0)
    {
        oi_setWheels(-230, 200); //Will make the Roomba turn in place (right wheel back and left wheel forward)
        while (turn >= degrees)
        { //NOTE TEST WITHOUT THE EQUAL AT SOME POINT
            oi_update(sensor_data);
            turn += sensor_data->angle;
        }
    }
    else
    {
        oi_setWheels(230, -200); //Will make the Roomba turn in place (right wheel forward and left wheel back)
        while (turn <= degrees)
        { //NOTE TEST WITHOUT THE EQUAL AT SOME POINT
            oi_update(sensor_data);
            turn += sensor_data->angle;
        }
    }
    oi_setWheels(0, 0); //stops movement
}

void load_songs() //load the songs
{

    unsigned char ImperialMarchNumNotes = 19;
    unsigned char ImperialMarchNotes[19] = { 55, 55, 55, 51, 58, 55, 51, 58, 55,
                                             0, 62, 62, 62, 63, 58, 54, 51, 58,
                                             55 };
    unsigned char ImperialMarchDurations[19] = { 32, 32, 32, 20, 12, 32, 20, 12,
                                                 32, 32, 32, 32, 32, 20, 12, 32,
                                                 20, 12, 32 };
    oi_loadSong(IMERPIAL_MARCH, ImperialMarchNumNotes, ImperialMarchNotes,
                ImperialMarchDurations);
    oi_play_song(1);

}

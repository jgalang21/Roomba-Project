
#include "ir.h"
#include "lcd.h"
#include "movement.h"
#include "open_interface.h"
#include "ping.h"
#include "servo.h"
#include "timer.h"
#include "uart.h"
#include <math.h>

typedef struct
{
    int start;
    int end;
    float dist; //middle
    float width;
    float startDist;
    float endDist;
} Data;

//modified this code, got rid of the width calculation sine they weren't necessary
void scanArea(void)//ScanData* objects, size_t len)
{
    int degree = 0;
    int numavg = 2;
    int i = 0;
    float ir_dist;
    Data objects[10]; //array of objects
    char UARTsend[100];

    //  Data objectArray[11];

    int p = 0;
    while (p < 5)
    {
        objects[p].start = 0; //start degree
        objects[p].end = 0; //end degree
        objects[p].dist = 0.0; //middle distance
        objects[p].width = 0.0; //width of the object
        objects[p].startDist = 0.0; //start distance
        objects[p].endDist = 0.0; //end distance
        p++;
    }

//check for things being overwritten (?)
//test movement and scan
    float tempDist = 0;
    int numSample = 0; //number of samples taken
    int hasHit = 0; //whether we're looking at an object or not
    int q = 0;
    //ping is better for distance and IR is better for detecting objects
    for (q = 0; q < 90; q++)
    {



        degree = q * 2;
        servo_move(degree); //move the servo two degrees each time

        int m = 0;
        ir_dist = 0.0; //initialize ir distace to 0
        for (m = 0; m < numavg; m++)
        { //average the IR distance to account for ghost objects
            ir_dist += ir_getDistance(); //grab the IR distance
        } //fix for final project

        ir_dist /= numavg;

        if (ir_dist < 45) //if one of the distances is < 40 and has not detected an edge
        {
            if (objects[i].start == 0 && hasHit == 0)
            { //if the object is in range and hasn't been found
                objects[i].start = degree; //find the start degree
                objects[i].startDist = ping_getDistance(); //find the distance from the start
                hasHit = 1; //say that the object has been hit
                numSample = 1; //start sampling
            }

            else
            { //if we're currently looking at an object
                numSample++; //sample once
                if(numSample > 2 && numSample < 7){
                    tempDist += ping_getDistance();
                }

            }

            //otherwise it shouldn't do anything
        }

        else if (ir_dist - ping_getDistance() >= 9.0 && hasHit == 1
                && numSample >= 5) //is
        { //if there's a giant gap between the previous and ir_distance then we've reached the end
            objects[i].end = degree; //store end degree;
            objects[i].endDist = ping_getDistance();
            objects[i].dist = tempDist/4;

            float conversion = ((3.1415926535) / 360.0);
            float toRadians = ((objects[i].end - objects[i].start)) * conversion
                    * 0.5; //converts to radians
            int angWidth = objects[i].end - objects[i].start; //angular width
            float angWidthRad = angWidth * conversion; //convert angular width
            float rVal = (((objects[i].startDist / sinf(1.5708 - (angWidthRad / 2)))
                    + (objects[i].endDist / sinf(1.5708 - (angWidthRad / 2)))));
            //law of sines https://math.stackexchange.com/questions/72666/how-to-get-apparent-linear-diameter-from-angular-diameter
            //https://en.wikipedia.org/wiki/Law_of_sines

            float tanVal = tanf(toRadians); //use the tangent
            objects[i].width = 2.0 * (rVal * (tanVal)); //get the width of the object
            hasHit = 0;
            i++;
            numSample = 0;
            tempDist = 0;

        }

        //debugging


        //stuff to send to putty
        sprintf(UARTsend, "%d\t\t%4.4f\t\t%4.4f \t %d \t %d\n\r", degree, ir_dist, ping_getDistance(), objects[i].start, objects[i].end); //convert float to char order is degrees ir ping
             uart_sendStr(UARTsend); //send to putty
    }



    int s = 0;
    uart_sendStr("Object\tDegree\tDistance\t\Width\n\r");
    for(s = 0; s< i; s++){
       sprintf(UARTsend, "%d\t %d\t %4.4f \t %4.4f\n\r", s+1, objects[s].start, objects[s].dist, objects[s].width); //convert float to char order is degrees ir ping
       uart_sendStr(UARTsend); //send to putty
       }

    servo_move(90);


}

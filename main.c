#include "ir.h"
#include "lcd.h"
#include "movement.h"
#include "open_interface.h"
#include "ping.h"
#include "servo.h"
#include "timer.h"
#include "uart.h"
#include "scan.h"
#include <math.h>


int end = 0;
int ManualCtl = 1;

void master_init(void)
{
    ir_init();
    lcd_init();
    ping_init();
    servo_init();
    uart_init();

}

void getCommand(oi_t*sensor_data, char c)
{

    oi_t* sensor = oi_alloc();
    oi_init(sensor);

    switch (c)
    {

    case 'e': //scan the area
    case 'E':
        uart_sendStr("Starting Scan...\r\n");
        scanArea(); //changed scan to only OUTPUT, no need to return anything
        break;

    case 'w': //move 20 cm forward
    case 'W':
        move_forward(sensor_data, 200);
        uart_sendStr("Moving Forward 20cm...\r\n");
        break;
    case 'i':
    case 'I'://move 10 cm forward
        move_forward(sensor_data, 100);
        uart_sendStr("Moving Forward 10cm...\r\n");
        break;
        //S is pressed
    case 's':
    case 'S':
        move_back(sensor_data, 200);  //move 20 cm back
        uart_sendStr("Moving Back 20 cm...\r\n");
        break;
        //A is pressed

    case 'a':
    case 'A':
        turn(sensor_data, 90); //rotate 90 degrees left
        uart_sendStr("Turning Left 90 degrees..\r\n");
        break;
        //Z is pressed

    case 'z':
    case 'Z':
        turn(sensor_data, 45); //rotate 45 degrees left
        uart_sendStr("Turning 45 Left...\r\n");
        break;

        //D is pressed
    case 'd':
    case 'D':
        turn(sensor_data, -90); //rotate 90 degrees right
        uart_sendStr("Turning Right...\r\n");
        break;
        //C is pressed
    case 'c':
    case 'C':
        turn(sensor_data, -45); //rotate 45 degrees right
        uart_sendStr("Turning 45 right...\r\n");
        break;
        //X is pressed
    case 'x':
    case 'X':
        turn(sensor_data, 90); //rotate 180 degrees behind it
        turn(sensor_data, 90); //rotate 180 degrees behind it
        uart_sendStr("Turning Around...\r\n");
        break;
        //Q is pressed
    case 'l':
    case 'L':
        turn(sensor_data, -15); //rotate 15 degrees right
        uart_sendStr("Turning 15 right...\r\n");
        break;
    case 'j':
    case 'J':
        turn(sensor_data, 15); //rotate 15 degrees left
        uart_sendStr("Turning 15 left...\r\n");
        break;
    case 'f':
    case 'F':
        end = 1; //end program
        uart_sendStr("Bot turned off\r\n");
        lcd_printf(
                "Reeeeeee \ncranberry sprite\nPlexa Dlay AespacitoLmash Lhat Bike Lttn");
        oi_free(sensor_data);
        break;
    case 'r':
    case 'R': //play imperial march
        load_songs();
        break;
    default:
        uart_sendStr("Unknown command, try again...\r\n");
        break;
    }
}

//here's a really good reference https://www.youtube.com/watch?v=OU5m58bhZ6Y
void main()
{
    char command = '\0';
    master_init(); //init all functions

    //open interface init
    oi_t* sensor_data = oi_alloc();
    oi_init(sensor_data);

    uart_interface(); //display putty interface

    while (!end)
    { //Main loop for manual control

        if (command == '\0' && ManualCtl)
        {
            command = uart_receive(); //receive the command
            getCommand(sensor_data, command); //executes command based off of the letter
            command = '\0'; //resets so same command doesnt loop infinitely
        }

    } //end switch case
    oi_free(sensor_data);
}
//  }//end main loop

//}//end of Main Function


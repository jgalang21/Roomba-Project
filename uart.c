/*
 *
 *   uart.c
 *   @author
 *   @date
 */

#include "uart.h"
#include <stdio.h>
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

void uart_init(void){
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1; //enable clock to GPIO, R1 = port B
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R1; //enable clock to UART1, R1 = UART1
    GPIO_PORTB_AFSEL_R |= (BIT0 | BIT1);//enable alternate functions on port b pins 0 and 1
    GPIO_PORTB_PCTL_R |= 0x00000011;//enable Rx and Tx on port B on pins 0 and 1
    GPIO_PORTB_DEN_R |= (BIT0 | BIT1);		 //set pin 0 and 1 to digital
    GPIO_PORTB_DIR_R &= ~BIT0;				 //set pin 0 to Rx or input
    GPIO_PORTB_DIR_R |= BIT1;				 //set pin 1 to Tx or output

    uint16_t iBRD = 8; //integer baud rate
    uint16_t fBRD = 44;  //fractional baud rate

    UART1_CTL_R &= ~(UART_CTL_UARTEN);		 //turn off uart1 while we set it up
    UART1_IBRD_R = iBRD;					 //set baud rate
    UART1_FBRD_R = fBRD;					 //set baud rate
    UART1_LCRH_R = UART_LCRH_WLEN_8;//set frame, 8 data bits, 1 stop bit, no parity, no FIFO
    UART1_CC_R = UART_CC_CS_SYSCLK;			 //use system clock as source

    //re-enable enable RX, TX, and uart1
    UART1_CTL_R = (UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN);
}

void uart_sendChar(char data){
    while (UART1_FR_R & 0x20){} //wait until there is room to send data

    UART1_DR_R = data;
}

char uart_receive(void){
    char oof = 0;

    while (UART1_FR_R & UART_FR_RXFE){} //wait for data to arrive

    oof = (char) (UART1_DR_R & 0xFF);

    return oof;
}

void uart_sendStr(const char *data){
    //While not equal to null
    while (*data != '\0'){
        uart_sendChar(*data);
        data++;
    }
}

void uart_interface(){
    uart_sendStr("*********************************\r\n");
    uart_sendStr("**VORTEX Mission Control Interface v1.1**\r\n");
    uart_sendStr("*********************************\r\n");
    uart_sendStr("List of available commands:\r\n");
    uart_sendStr("*********************************\r\n");
    uart_sendStr("E - Scan Area\r\n");
    uart_sendStr("W - Move forward\r\n");
    uart_sendStr("S - Move Backward\r\n");
    uart_sendStr("A - Turn 90 Degrees Left (Counterclockwise)\r\n");
    uart_sendStr("Z - Turn 45 Degrees Left (Counterclockwise)\r\n");
    uart_sendStr("D - Turn 90 Degrees Right (clockwise)\r\n");
    uart_sendStr("C - Turn 45 Degrees Right (clockwise)\r\n");
    uart_sendStr("X - Turn 180 Degrees\r\n");
    uart_sendStr("I - Move slightly forward\r\n");
    uart_sendStr("J - Move slightly Left\r\n");
    uart_sendStr("K - Move slightly Right\r\n");
    uart_sendStr("X - Turn 180 Degrees\r\n");
    uart_sendStr("R - Play a song\r\n");
    uart_sendStr("F - End program (end)\r\n");
    uart_sendStr("*********************************\r\n");
    uart_sendStr("Enter a command:\r\n");

}

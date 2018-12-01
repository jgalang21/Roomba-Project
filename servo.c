/*
 * servo.c
 *
 *  Created on: Oct 17, 2018
 *      Author: ecpower
 */

#include "servo.h"
#include "timer.h"
#include "lcd.h"


//bot0 29000=180,400=0
//bot1 28000=180,900=0
#define MaxPW 28000.0 //calibration value for 180 degrees
#define MinPW 900.0 //calibration value for 000 degrees
#define TMax 32000.0 //theoretical Max
#define TMin 16000.0 //theoretical Min

unsigned period_length = 320000.0; //pulse period in cycles 20ms
volatile signed pulse_width = 0.0;

void servo_init(){
    //Initialize GPIO PB5
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;    //Port B
    GPIO_PORTB_AFSEL_R |= BIT5;   //Uses alternate hardware
    GPIO_PORTB_PCTL_R |= 0x700000;
    GPIO_PORTB_DEN_R |= 0X20;   //Set digital enable
    GPIO_PORTB_DIR_R |= 0x20;   //Set pin 5 as an output

    //Initialize Timer1
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1;  //Turn on clock for timer1
    TIMER1_CTL_R &=  ~(TIMER_CTL_TBEN); //Disable timer to configure
    TIMER1_CFG_R |= TIMER_CFG_16_BIT;  //set size of timer to 16
    TIMER1_TBMR_R |= (TIMER_TBMR_TBMR_PERIOD | TIMER_TBMR_TBAMS);  //periodic and PWM enable
    TIMER1_TBMR_R &= ~(TIMER_TBMR_TBCMR);
    TIMER1_TBMR_R &= 0xFFFFFFEF; //count down pwm
    TIMER1_TBMR_R |= 0x800; //CCP driven by high
    TIMER1_CTL_R &= ~(TIMER_CTL_TBPWML);  //set to non-inverted PWM mode
    TIMER1_TBILR_R |= (period_length & 0xFFFF); // set lower 16 bits of interval
    TIMER1_TBPR_R |= (period_length >> 16); //set upper 8 bits of interval
    servo_move(90);
    TIMER1_CTL_R |= TIMER_CTL_TBEN;  //Enable timer

}

void servo_move(int degree){
    signed PW;
    PW = ((degree/180.0)*(period_length/20.0)+(period_length/20.0));

    pulse_width = MinPW+((PW-TMin)*(MaxPW-MinPW)/(TMax-TMin));

    if (pulse_width > MaxPW){pulse_width=MaxPW;} //Upper Bound
    if (pulse_width < MinPW){pulse_width=MinPW;} //Lower bound

    TIMER1_TBMATCHR_R = ((period_length-pulse_width) & 0xFFFF);   //Lower 16 bits of the interval
    TIMER1_TBPMR_R = ((period_length-pulse_width) >> 16); //Set upper 8 bits of the interval

    timer_waitMillis(pulse_width/16000.0);
    timer_waitMillis(20-(pulse_width/16000.0));
}

/*void servo_calibrate(void){
    int degree = 90;
    int direction = 1;
    int button = 0;
    int count = 0;

    servo_init();
    button_init();
    lcd_init();

    while(1){
        button = button_getButton();
        if(count==4){count=0;}

        if (button == 6){
            //Do nothing
        }
        if (button == 5){
            //Do nothing
        }
        if (button == 4){
            direction *= -1;
        }
        if (button == 3){
            degree += 5*direction;
        }
        if (button == 2){
            degree += 2.5*direction;
        }
        if (button == 1){
            degree += 1*direction;
        }
        count++;
        timer_waitMillis(200);
        servo_move(degree);
        lcd_printf("Direction = %d\nDegrees = %d\nPW = %d",direction,degree,pulse_width);
    }
}*/

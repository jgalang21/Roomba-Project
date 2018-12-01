/*
*   ping.c
*/
#define BIT0        0x01
#define BIT1        0x02
#define BIT2        0x04
#define BIT3        0x08
#define BIT4        0x10
#define BIT5        0x20
#define BIT6        0x40
#define BIT7        0x80
#define clock_speed 16000000.0 //16mill Hz
#define sound_speed 34000.0    //Speed of sound cm/s

volatile unsigned rising_time = 0;
volatile unsigned falling_time = 0;
volatile enum {Done, Rising_Edge, Falling_Edge} cycle;
volatile signed difference = 0;
volatile int overflow = 0;

#include <ping.h>
#include <timer.h>

void TIMER3B_Handler(void){
       if(cycle == Rising_Edge){ //Checks for the rising edge of the clock cycle.

           TIMER3_ICR_R = TIMER_ICR_CBECINT; //clear capture interrupt flag
           rising_time = TIMER3_TBR_R; //Captures current time.
           cycle = Falling_Edge; //Switches to watch for the second clock edge.
       }

       //Checks for the falling edge of the clock cycle.
       else if(cycle == Falling_Edge){

           TIMER3_ICR_R = TIMER_ICR_CBECINT;  //clear capture interrupt flag
           falling_time = TIMER3_TBR_R;  //Captures current time.
           cycle = Done;
       }
   }


void ping_init(void){
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R3;  //Turn on clock to TIMER3
    SYSCTL_RCGCGPIO_R |= 0x02; //Port B (bit1) system clock
    GPIO_PORTB_DEN_R |= 0x08;  //Pin 3 init
    GPIO_PORTB_PCTL_R |= GPIO_PCTL_PB3_T3CCP1;

    //Timer
    TIMER3_CTL_R &= ~TIMER_CTL_TBEN; //Disable timer
    TIMER3_CTL_R |= TIMER_CTL_TBEVENT_BOTH;  //Edge select
    TIMER3_CFG_R |= TIMER_CFG_16_BIT;  //Set to a 16 bit counter
    TIMER3_TBMR_R |= TIMER_TBMR_TBMR_CAP;  //Capture mode
    TIMER3_TBMR_R |= TIMER_TBMR_TBCMR;  //Edge-time mode
    TIMER3_TBMR_R |= TIMER_TBMR_TBCDIR;  //count up
    TIMER3_TBILR_R |= 0xFFFF;  //Set upper bound
    TIMER3_IMR_R |= TIMER_IMR_CBEIM;  //enable capture input
    TIMER3_ICR_R |= TIMER_ICR_CBECINT;  //clears interrupt flags

    NVIC_EN1_R |= 0x10;  //Timer 3B: 36 Register 5, set Enable (EN1) BIT4 or 0x10
    TIMER3_CTL_R |= TIMER_CTL_TBEN; //Enable timer

    IntRegister(INT_TIMER3B, TIMER3B_Handler);  //register TIMER3B interrupt handler
    IntMasterEnable();  //initialize global interrupt
}

void send_pulse(void){
        TIMER3_CTL_R &= ~TIMER_CTL_TBEN; //Disable timer

        GPIO_PORTB_AFSEL_R &= ~(0x08);  //Disables alternate function for PORTB.
        GPIO_PORTB_DIR_R |= 0x08;  //Set PB3 as output.
        GPIO_PORTB_DATA_R |= 0x08;  //Set PB3 to high.
        timer_waitMillis(5);  //Delays for 5 micro seconds.
        GPIO_PORTB_DATA_R &= 0xF7; //Set PB3 to low.
        GPIO_PORTB_DIR_R &= 0xF7;  //Set PB3 as input.
        GPIO_PORTB_AFSEL_R |= 0x08; //Re-enables alternate function for portB.

        TIMER3_CTL_R |= TIMER_CTL_TBEN; //Enable timer
}

float ping_getDistance(void){
    send_pulse();  //Sends sonar pulse.
    cycle = Rising_Edge; //Sets the state machine to look for the rising edge of the clock cycle.

    while(cycle != Done){};
    if(rising_time > falling_time){ //Calculate & adjusts for the overflow of the system.
        //falling_time += ((unsigned) pow(2,24) - 1 - rising_time);
        difference = (overflow << 24) + difference; //Equivalent of shifting the bit by 24 positions to the left. Subtracts appropriate amount to correct the value.

        //Increments the overflow.
        overflow = overflow + 1;
    }
    else{
        //Subtracts the rising and falling edge to get the difference between the two clock values.
        difference = falling_time - rising_time;
    }
    //Returns the calculated distance in cm.
    float distance = ((difference/2.0) * (sound_speed / clock_speed));
    return  distance;
}

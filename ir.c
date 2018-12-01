/*
*
*   ir.c
*
*/

#include <ir.h>
#include <math.h>
#include <timer.h>



void ir_init(void){
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;  //enable ADC 0 module on port D
    SYSCTL_RCGCADC_R |= 0x1;  //enable clock for ADC
    GPIO_PORTB_AFSEL_R  |= 0x01; //enable port D pin 0 to work as alternate functions
    GPIO_PORTB_DIR_R &= 0b11111110; //set pin to input - 0
    GPIO_PORTB_DEN_R |= 0x00; //disable PB4 digital function
    GPIO_PORTB_AMSEL_R |= 0x01;  //disable analog isolation for the pin
    GPIO_PORTB_ADCCTL_R = 0x00; //initialize the port trigger source as processor (default)

    ADC0_ACTSS_R &= ~ADC_ACTSS_ASEN1;  //disable SS1 sample sequencer to configure it
    ADC0_EMUX_R = ADC_EMUX_EM1_PROCESSOR;  //initialize the ADC trigger source as processor (default)
    ADC0_SSMUX1_R |= 0x000A; //set 1st sample to use the AIN10 ADC pin
    ADC0_SSCTL0_R |= (ADC_SSCTL1_IE0 | ADC_SSCTL1_END0); //enable raw interrupt status
    ADC0_SAC_R |= ADC_SAC_AVG_64X;  //enable oversampling to average
    ADC0_ACTSS_R |= ADC_ACTSS_ASEN1;  //re-enable ADC0 SS1
}

int ir_read(void){
    ADC0_ACTSS_R &= ~ADC_ACTSS_ASEN1;  //disable ADC0SS1 sample sequencer to configure it
    ADC0_SSMUX1_R |= 0;  //set 1st sample to use the channel ADC pin
    ADC0_ACTSS_R |= ADC_ACTSS_ASEN1; //re-enable ADC0 SS1
    ADC0_PSSI_R=ADC_PSSI_SS1;  //initiate SS1 conversion

    while((ADC_RIS_INR1) == 0){}  //wait for ADC conversion to be complete
    ADC0_ISC_R=ADC_ISC_IN1; //clear interrupt

    return ADC0_SSFIFO1_R;
}

float ir_getDistance(void){
    int i = 0, increments = 100;
    isr_int_value = 0;

    //Compute a running average of 100 samples and display that value
    for(i=0;i<increments;i++){
        timer_waitMicros(500);  //samples every 1 millisecond
        isr_int_value += ir_read();
    }
    int value = isr_int_value/increments; //average of samples

    //equation from excel y = 105375 x ^(-1.153) where y = distance (cm) and x = quantized value
    float distance = 105375 * pow(value,-1.153);
    return distance;
}

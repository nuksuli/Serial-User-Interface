/*
 * File:   led.c
 * Author: jepino
 *
 * Created on 12 December 2020, 17:49
 */


#define F_CPU   3333333

#include <avr/io.h>

void LED_pwm(uint8_t duty)
{
    //Disable TCA
    TCA0.SPLIT.CTRLA &= ~(TCA_SPLIT_ENABLE_bm);
    //Set duty
    TCA0.SPLIT.HCMP2 = UINT8_MAX - duty;
    //Select prescaler of 16 and start timer
    TCA0.SPLIT.CTRLA = TCA_SPLIT_CLKSEL_DIV16_gc | TCA_SPLIT_ENABLE_bm;
}

//Turn on LED (PF5)
void LED_on(void)
{
    //Disable TCA
    TCA0.SPLIT.CTRLA &= ~(TCA_SPLIT_ENABLE_bm);
    TCB3.CTRLB &= ~(TCB_CCMPEN_bm);
    PORTF.OUTCLR = PIN5_bm;
}

//Turn off LED (PF5)
void LED_off(void)
{
    //Disable TCA
    TCA0.SPLIT.CTRLA &= ~(TCA_SPLIT_ENABLE_bm);
    PORTF.OUTSET = PIN5_bm;
}
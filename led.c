/*
 * File:   led.c
 * Author: dtek0068
 *
 * Created on 12 December 2020, 17:49
 */


#define F_CPU   3333333

#include <avr/io.h>
#include <util/delay.h>


//Turn on LED (PF5)
void LED_on(void)
{
    PORTF.OUTCLR = PIN5_bm;
}

//Turn off LED (PF5)
void LED_off(void)
{
    PORTF.OUTSET = PIN5_bm;
}

void LED_status(void)
{
    if (PORTF.OUT & PIN5_bm)
        {
            printf("LED status: OFF\r\n");
        }
        else 
        {
            printf("LED status: ON\r\n");
        }
}

//Set led on with duty
void pwm_period(int duty)
{        
    while(1)
    {
        
    
        for (unsigned int i = 0; i < (255 - duty) ; i++)
        {
            _delay_us(7);
        }

        PORTF.OUTSET = PIN5_bm;

        for (unsigned int i = 0; i < duty  ; i++)
        {
            _delay_us(7);
        }


        PORTF.OUTCLR = PIN5_bm;
    }
}

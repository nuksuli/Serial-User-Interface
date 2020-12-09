/*
 * File:   vref.c
 * Author: Werneri Aarnio
 *
 * Created on 09 December 2020, 23:35
 */


#include <avr/io.h>
#include <string.h>

void set_vref(char vrefvalue[])
{   
    
    if (strcmp(vrefvalue, "1V1") == 0)
    {
        //Set reference voltage to 1.1V
        VREF.CTRLA |= VREF_ADC0REFSEL_1V1_gc;
    }
    if (strcmp(vrefvalue, "0V55") == 0)
    {
        //Set reference voltage to 0.55V
        VREF.CTRLA |= VREF_ADC0REFSEL_0V55_gc;
    }
    if (strcmp(vrefvalue, "1V5") == 0)
    {
        //Set reference voltage to 1.5V
        VREF.CTRLA |= VREF_ADC0REFSEL_1V5_gc;
    }
    if (strcmp(vrefvalue, "2V5") == 0)
    {
        //Set reference voltage to 2.5V
        VREF.CTRLA |= VREF_ADC0REFSEL_2V5_gc;
    }
    
}
char get_vref(void)
{
    return VREF.CTRLA;
}

/*
 * File:   vref.c
 * Author: Werneri Aarnio
 *
 * Created on 09 December 2020, 23:35
 */


#include <avr/io.h>
#include <string.h>

int set_vref(char vrefvalue[])
{   
    
    if (strcmp(vrefvalue, "1V1") == 0)
    {
        //Set reference voltage to 1.1V
        VREF.CTRLA |= VREF_AC0REFSEL_1V1_gc;
        return 1;
    }
    else if (strcmp(vrefvalue, "0V55") == 0)
    {
        //Set reference voltage to 0.55V
        VREF.CTRLA |= VREF_AC0REFSEL_0V55_gc;
        return 1;
    }
    else if (strcmp(vrefvalue, "1V5") == 0)
    {
        //Set reference voltage to 1.5V
        VREF.CTRLA |= VREF_AC0REFSEL_1V5_gc;
        return 1;
    }
    else if (strcmp(vrefvalue, "2V5") == 0)
    {
        //Set reference voltage to 2.5V
        VREF.CTRLA |= VREF_AC0REFSEL_2V5_gc;
        return 1;
    }
    else
    {
        return 0;
    }
    
}
char get_vref(void)
{
    return VREF.CTRLA;
}

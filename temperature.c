/*
 * File:   temperature.c
 * Author: Niko Kalliomaa
 *
 * Created on 09 December 2020, 22:13
 */


#include <avr/io.h>
#include "temperature.h"

uint16_t temperature(void)
{
    //Set reference voltage to 1.1V
    VREF.CTRLA |= VREF_ADC0REFSEL_1V1_gc;
    //Set temperature sensor channel
    ADC0.MUXPOS |= ADC_MUXPOS_TEMPSENSE_gc;
    
    ADC0.CTRLD |= ADC_INITDLY_DLY64_gc;
    ADC0.SAMPCTRL = 64;
    ADC0.CTRLC |= ADC_SAMPCAP_bm;
    ADC0.CTRLA |= ADC_ENABLE_bm;
    ADC0.COMMAND = ADC_STCONV_bm;
    // wait for completion
    while ((ADC0.INTFLAGS & ADC_RESRDY_bm) == 0)
    {
        ;
    }
    // Read signed value from signature row 
    int8_t sigrow_offset = SIGROW.TEMPSENSE1; 
    // Read unsigned value from signature row 
    uint8_t sigrow_gain = SIGROW.TEMPSENSE0; 
    // ADC conversion result with 1.1 V internal reference 
    uint16_t adc_reading = ADC0.RES;
    // Result might overflow 16 bit variable (10bit+8bit)
    uint32_t temp = adc_reading - sigrow_offset; 
    temp *= sigrow_gain; 
    // Add 1/2 to get correct rounding on division below 
    temp += 0x80; 
    // Divide result to get Kelvin
    temp >>= 8; 
    // Get celsius by lovering the value by 273
    uint16_t temperature_in_C = temp - 273;
    
    return temperature_in_C;
}

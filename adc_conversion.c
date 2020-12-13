/*
 * File:   adc_conversion.c
 * Author: Niko Kalliomaa
 *
 * Created on 09 December 2020, 23:42
 */


/* RTC Period */
#define RTC_PERIOD            (511)

#include <avr/io.h>
#include <avr/interrupt.h>


void ADC0_init();
uint16_t ADC0_read(void);
volatile uint8_t channel = 0;

void ADC0_init()
{
    /* Disable digital input buffer */
    PORTD.PIN6CTRL &= ~PORT_ISC_gm;
    PORTD.PIN6CTRL |= PORT_ISC_INPUT_DISABLE_gc;
    
    /* Disable pull-up resistor */
    PORTD.PIN6CTRL &= ~PORT_PULLUPEN_bm;

    ADC0.CTRLC = ADC_PRESC_DIV4_gc      /* CLK_PER divided by 4 */
               | ADC_REFSEL_INTREF_gc;  /* Internal reference */
    
    ADC0.CTRLA = ADC_ENABLE_bm          /* ADC Enable: enabled */
               | ADC_RESSEL_10BIT_gc;   /* 10-bit mode */
    
    /* Select ADC channel */
    
    if (channel == 0)
    {
        ADC0.MUXPOS  = ADC_MUXPOS_AIN0_gc;
    }
    else if (channel == 1)
    {
        ADC0.MUXPOS  = ADC_MUXPOS_AIN1_gc;
    }
    else if (channel == 2)
    {
        ADC0.MUXPOS  = ADC_MUXPOS_AIN2_gc;
    }
    else if (channel == 3)
    {
        ADC0.MUXPOS  = ADC_MUXPOS_AIN3_gc;
    }
    else if (channel == 4)
    {
        ADC0.MUXPOS  = ADC_MUXPOS_AIN4_gc;
    }
    else if (channel == 5)
    {
        ADC0.MUXPOS  = ADC_MUXPOS_AIN5_gc;
    }
    else if (channel == 6)
    {
        ADC0.MUXPOS  = ADC_MUXPOS_AIN6_gc;
    }
    else if (channel == 7)
    {
        ADC0.MUXPOS  = ADC_MUXPOS_AIN7_gc;
    }
    else if (channel == 8)
    {
        ADC0.MUXPOS  = ADC_MUXPOS_AIN8_gc;
    }
    else if (channel == 9)
    {
        ADC0.MUXPOS  = ADC_MUXPOS_AIN9_gc;
    }
    else
    {
        ADC0.MUXPOS = ADC_MUXPOS_AIN6_gc;
    }
}
uint16_t ADC0_read(void)
{
    /* Start ADC conversion */
    ADC0.COMMAND = ADC_STCONV_bm;
    
    /* Wait until ADC conversion done */
    while ( !(ADC0.INTFLAGS & ADC_RESRDY_bm) )
    {
        ;
    }
    
    /* Clear the interrupt flag by writing 1: */
    ADC0.INTFLAGS = ADC_RESRDY_bm;
    
    return ADC0.RES;
}

uint16_t ADC0_conversion(void) {
    ADC0_init();
    return ADC0_read();
}


uint8_t ADC0_get_channel(void)
{
    return channel;
}

uint8_t ADC0_set_channel(uint8_t ch)
{
    if ((ch <= 0) && (ch <= 16))
    {
        channel = ch;
        return 1;
    }    
    else {
        return 0;
    }
}
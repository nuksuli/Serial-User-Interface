/*
 * File:   main.c
 * Author: Niko Kalliomaa, Jerri Pinomäki, Werneri Aarnio and Matias Lappalainen
 *
 * Created on 09 December 2020, 16:02
 */
#define USART0_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)
#define F_CPU 3333333

#include <avr/io.h>
#include <avr/sleep.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>


static void USART0_sendChar(char c)
{
    while (!(USART0.STATUS & USART_DREIF_bm))
    {
        ;
    }
    USART0.TXDATAL = c;
}

static int USART0_printChar(char c, FILE *stream)
{ 
    USART0_sendChar(c);
    return 0; 
}

static FILE USART_stream = FDEV_SETUP_STREAM(USART0_printChar, 
        NULL, _FDEV_SETUP_WRITE);

//initialize USART
static void USART0_init(void)
{
    PORTA.DIR |= PIN0_bm;
    
    USART0.BAUD = (uint16_t)USART0_BAUD_RATE(9600); 
    
    USART0.CTRLB |= USART_TXEN_bm;  
    
    stdout = &USART_stream;
}

int main(void) {
    /* Replace with your application code */
    while (1) {
        printf("send message!");
        _delay_ms(500);
    }
}

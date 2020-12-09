/*
 * File:   reset.c
 * Author: Niko Kalliomaa and Matias Lappalainen
 *
 * Created on 09 December 2020, 23:03
 */


#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>


void reset(void)
{
    wdt_enable(WDTO_2S);
    wdt_reset();
}

ISR(WDT_vect) {
  // DNothing on here, but this is
  // a block of code otherwise the interrupt calls an
  // uninitialized interrupt handler.
}
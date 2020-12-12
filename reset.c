/*
 * File:   reset.c
 * Author: Niko Kalliomaa and Matias Lappalainen
 *
 * Created on 09 December 2020, 23:03
 */


#include <avr/io.h>


void reset(void)
{
    _PROTECTED_WRITE(RSTCTRL.SWRR,RSTCTRL_SWRE_bm);
}
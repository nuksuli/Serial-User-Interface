/*
 * File:   reset.c
 * Author: dtek0068
 *
 * Created on 09 December 2020, 23:03
 */


#include <avr/io.h>


void reset(void)
{
    _PROTECTED_WRITE(RSTCTRL.SWRR,RSTCTRL_SWRE_bm);
}
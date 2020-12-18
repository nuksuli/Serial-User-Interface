/*
 * File:   button.c
 * Author: jepino
 *
 * Created on 13 December 2020, 10:13
 */


#include <avr/io.h>

void BTN_set_invert()
{
    PORTF.PIN6CTRL |= PORT_INVEN_bm;
}

void BTN_clear_invert()
{
    PORTF.PIN6CTRL &= ~PORT_INVEN_bm;
}

void BTN_enable_pullup()
{
    PORTF.PIN6CTRL |= PORT_PULLUPEN_bm;
}

void BTN_disable_pullup()
{
    PORTF.PIN6CTRL &= ~PORT_PULLUPEN_bm;
}

char* BTN_status()
{
    char *status;
    if (PORTF.IN & PIN6_bm)
    {
        if (PORTF.PIN6CTRL & PORT_INVEN_bm)
        {
            if (PORTF.PIN6CTRL & PORT_PULLUPEN_bm)
            {
                status = "Button is HIGH, Invert is ENABLED, "
                         "Pull-up resistor is ENABLED\n\r";
            }
            else
            {
                status = "Button is HIGH, Invert is ENABLED, "
                         "Pull-up resistor is DISABLED\n\r";
            }
        }
        else
        {
            if (PORTF.PIN6CTRL & PORT_PULLUPEN_bm)
            {
                status = "Button is HIGH, Invert is DISABLED, "
                         "Pull-up resistor is ENABLED\n\r";
            }
            else
            {
                status = "Button is HIGH, Invert is DISABLED, "
                         "Pull-up resistor is DISABLED\n\r";
            }
        }
    }
    else
    {
        if (PORTF.PIN6CTRL & PORT_INVEN_bm)
        {
            if (PORTF.PIN6CTRL & PORT_PULLUPEN_bm)
            {
                status = "Button is LOW, Invert is ENABLED, "
                         "Pull-up resistor is ENABLED\n\r";
            }
            else
            {
                status = "Button is LOW, Invert is ENABLED, "
                         "Pull-up resistor is DISABLED\n\r";
            }
        }
        else
        {
            if (PORTF.PIN6CTRL & PORT_PULLUPEN_bm)
            {
                status = "Button is LOW, Invert is DISABLED, "
                         "Pull-up resistor is ENABLED\n\r";
            }
            else
            {
                status = "Button is LOW, Invert is DISABLED, "
                         "Pull-up resistor is DISABLED\n\r";
            }
        }
    }
    return status;
}
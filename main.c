/*
 * File:   main.c
 * Author: jepino, nikall, waaarn, majlap
 *
 * Created on 09 December 2020, 16:02
 */

#define F_CPU 3333333
#define USART0_BAUD_RATE(BAUD_RATE) \
        ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)
#define MAX_COMMAND_LEN 255
#define MAX_ARGUMENT_LEN 3
#define BACKSPACE 127
#define VREF_0V55 0b00000000
#define VREF_1V1 0b00010001
#define VREF_1V5 0b01000100
#define VREF_2V5 0b00100010

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "temperature.h"
#include "reset.h"
#include "adc_conversion.h"
#include "vref.h"
#include "led.h"
#include "button.h"
#include <avr/sleep.h>

/* Function prototypes */
void USART0_init(void);
void PIN_init(void);
void TCA0_init(void);
void TCA0_hard_reset(void);
void command_parse(char **parsed_command, char *command);
void command_execute(char **parsed_command);
static FILE USART_stream;

/* Declaration of global variables */
char *command;
char **parsed_command;
uint8_t command_pointer = 0;

/* Initialise serial data transfer */
void USART0_init(void)
{
    PORTA.DIRCLR = PIN1_bm;
    PORTA.DIRSET = PIN0_bm;

    USART0.CTRLA |= USART_RXCIE_bm;
    USART0.CTRLA &= ~USART_TXCIE_bm;

    USART0.BAUD = (uint16_t)USART0_BAUD_RATE(9600);

    USART0.CTRLB |= USART_RXEN_bm | USART_TXEN_bm;
    stdout = &USART_stream;
}

/* Print char tp serial */
static int USART0_charprint(char c, FILE *stream)
{
    while (!(USART0.STATUS & USART_DREIF_bm))
    {
        ;
    }
    USART0.TXDATAL = c;
    return 0;
}

/* Configure printf serial data transfer */
static FILE USART_stream = FDEV_SETUP_STREAM(USART0_charprint,
                                             NULL, _FDEV_SETUP_WRITE);
/* Hard reset TCA0 (recommended by microchip before changing to split mode)*/
void TCA0_hard_reset(void)
{
    /* stop timer */
    TCA0.SINGLE.CTRLA &= ~(TCA_SINGLE_ENABLE_bm);  
    
    /* force a hard reset */
    TCA0.SINGLE.CTRLESET = TCA_SINGLE_CMD_RESET_gc; 
}

/* Initialise TCA0 (output register, mode, resolution)*/
void TCA0_init(void)
{
    /* Set pwm output to PORTF */
    PORTMUX.TCAROUTEA = PORTMUX_TCA0_PORTF_gc;
    /* enable split mode */
    TCA0.SPLIT.CTRLD = TCA_SPLIT_SPLITM_bm;
    /* Set period resolution */
    TCA0.SPLIT.HPER = UINT8_MAX;
    /* Start timer */
    TCA0.SPLIT.CTRLA = TCA_SPLIT_ENABLE_bm;
}

/* Set I/O pins and  */
void PIN_init(void)
{   
    PORTF.DIRSET = PIN5_bm;
    PORTF.OUTSET = PIN5_bm;
    PORTF.DIRCLR = PIN6_bm;
}

/* Parse arguments separated by spaces from input string */
void command_parse(char **parsed_command, char *command)
{
    char *token;
    uint8_t i = 0;

    token = strtok(command, " ");
    while (token != NULL)
    {
        parsed_command[i++] = strdup(token);
        token = strtok(NULL, " ");
    }
}

/* Execute command defined by parsed arguments */
void command_execute(char **parsed_command)
{
    /* Block for handling LED commands */
    if (strcmp(parsed_command[0], "LED") == 0)
    {
        if (strcmp(parsed_command[1], "ON") == 0)
        {
            LED_on();
        }
        else if (strcmp(parsed_command[1], "OFF") == 0)
        {
            LED_off();
        }
        else if (strcmp(parsed_command[1], "SET") == 0)
        {
            int duty = strtol(parsed_command[2], NULL, 10);
            
            if ((duty < 0)||(duty > UINT8_MAX))
            {
                printf("DUTY NOT VALID\n\r");
            }
            else
            {
                LED_pwm((uint8_t) duty);
            }
        }
        else
        {
            if (TCA0.SPLIT.CTRLB & TCA_SPLIT_HCMP2EN_bm)
            {
                printf("LED is in PWM MODE. DUTY is set to %d\n\r", UINT8_MAX - TCA0.SPLIT.HCMP2);
            }
            else if (PORTF.OUT & PIN5_bm)
            {
                printf("LED is OFF\n\r");
            }
            else
            {
                printf("LED is ON\n\r");
            }
        }
    }
    /* Block for handling button commands */
    else if (strcmp(parsed_command[0], "BTN") == 0)
    {
        if (strcmp(parsed_command[1], "INV") == 0)
        {
            if (strcmp(parsed_command[2], "ON") == 0)
            {
                BTN_set_invert();
            }
            else if (strcmp(parsed_command[2], "OFF") == 0)
            {
                BTN_clear_invert();
            }
            else
            {
                printf("INVALID COMMAND\n\r");
            }
        }
        else if (strcmp(parsed_command[1], "PUP") == 0)
        {
            if (strcmp(parsed_command[2], "ON") == 0)
            {
                BTN_enable_pullup();
            }
            else if (strcmp(parsed_command[2], "OFF") == 0)
            {
                BTN_disable_pullup();
            }
            else
            {
                printf("INVALID COMMAND\n\r");
            }
        }
        else
        {
            printf("%s\n\r", BTN_status());
        }
    }
    /* Block for handling temp commands */
    else if (strcmp(parsed_command[0], "TEMP") == 0)
    {
        uint16_t temp = temperature();
        printf("Temperature: %d \n\r", temp);
    }
    /* Block for handling reset commands */
    else if (strcmp(parsed_command[0], "RESET") == 0)
    {
        printf("Resetting...\r\n");
        reset();
    }
    /* Block for handling ADC commands */
    else if (strcmp(parsed_command[0], "ADC") == 0)
    {
        if (strcmp(parsed_command[1], "SET") == 0)
        {
            if (strstr(parsed_command[2], "AN") != NULL)
            {
                uint8_t ch = strtol(parsed_command[2] + 2, NULL, 10);
                if (ADC0_set_channel(ch) == 1)
                {
                    printf("Channel set to: %i \r\n", ch);
                }
                else
                {
                    printf("Invalid channel\r\n");
                }
            }     
            else
            {
                printf("Invalid arguments!");
            }
        }
        else if(strcmp(parsed_command[1], "\0") == 0)
        {
            uint16_t adc = ADC0_conversion();
            printf("Conversion result: %d \n\r", adc);
        }
        else
        {
            printf("Invalid arguments!\r\n"
                    "Check HELP ADC for available commands.\r\n");
        }
    }
    /* Block for handling VREF commands */
    else if (strcmp(parsed_command[0], "VREF") == 0)
    {
        if (strcmp(parsed_command[1], "SET") == 0)
        {
            if (set_vref(parsed_command[2]) == 1)
            {
                printf("Success\n\r");
            }
            else
            {
                printf("Set failed\n\r");
            }
        }
        else
        {
            if (get_vref() == VREF_1V1)
            {
                printf("VREF VALUE IS 1.1V\n\r");
            }
            else if (get_vref() == VREF_0V55)
            {
                printf("VREF VALUE IS 0.55V\n\r");
            }
            else if (get_vref() == VREF_1V5)
            {
                printf("VREF VALUE IS 1.5V\n\r");
            }
            else if (get_vref() == VREF_2V5)
            {
                printf("VREF VALUE IS 2.5V\n\r");
            }
            else
            {
                printf("%d\n\r", get_vref());
            }
        }
    }
    /* Block for handling help commands */
    else if (strcmp(parsed_command[0], "HELP") == 0)
    {
        if (strcmp(parsed_command[1], "LED") == 0)
        {
            printf("Available LED commands:\n\r"
                   "\tLED\t\t print LED driver status\n\r"
                   "\tLED [ON|OFF]\t turn LED on or off\n\r"
                   "\tLED SET <n>\t set led brightness(0 <= n <= 255)\n\r");
        }
        else if (strcmp(parsed_command[1], "BTN") == 0)
        {
            printf("Available BTN commands:\n\r"
                   "\tBTN\t\t print button status\n\r"
                   "\tINV [ON|OFF]\t configure state invert\n\r"
                   "\tPUP [ON|OFF]\t configure pull-up resistor\n\r");
        }
        else if (strcmp(parsed_command[1], "TEMP") == 0)
        {
            printf("Available TEMP commands:\n\r"
                   "\tTEMP\t\t print internal temperature in Celsius\n\r");
        }
        else if (strcmp(parsed_command[1], "VREF") == 0)
        {
            printf("Available VREF commands: \n\r"
                   "\tVREF\t\t print VREF value\n\r"
                   "\tVREF SET <n>\t set VREF value\n\?"
                   "\tAvailable values:\n\r"
                   "\t[0V55, 1V1, 1V5, 2V5]\n\r");
        }
        else if (strcmp(parsed_command[1], "ADC") == 0)
        {
            printf("Analog-to-digital comparator.\n\r"
                   "Avajlable ADC commands: \n\r"
                   "\tADC\t print Analog-to-digital conversion\n\r"
                   "\tADC SET AN<n>\t set analog input channel\n\r"
                   "\tAvailable channels: 0-15\n\r");
        }
        else
        {
            printf("Available commands:\n\r"
                   "\tLED \tLED Settings (HELP LED for Details)\n\r"
                   "\tBTN \tButton Settings (HELP BTN for Details)\n\r"
                   "\tHELP\tThis Help\n\r"
                   "\tRESET\tReset the microcontroller\n\r"
                   "\tVREF\tVREF Settings (HELP VREF for Details)\n\r"
                   "\tADC\tADC Settings (HELP ADC for Details\n\r");
        }
    }
    else
    {
        printf("NOT A VALID COMMAND!\r\n");
    }
    /* Reset command variable */
    for (uint8_t i = 0; i < UINT8_MAX; i++)
    {
        parsed_command[i] = "\0";
    }
}

int main(void)
{
    USART0_init();
    TCA0_hard_reset();
    TCA0_init();
    PIN_init();
    /* Allocate memory for global variables (Never freed but reused)*/
    command = malloc(UINT8_MAX * sizeof(char));
    parsed_command = malloc(5 * sizeof(char *));
    /* Configure IDLE sleep mode */
    set_sleep_mode(SLPCTRL_SMODE_IDLE_gc);
    /* Enable interrupts */
    sei();

    printf("Program starting!\r\n");

    /* Superloop calls sleep mode repeatedly, ISR handles functionality */
    while (1)
    {
        sleep_mode();
    }
}

/* ISR for USART0 input, handles calling command parser and executer */
ISR(USART0_RXC_vect)
{
    char next_char = USART0.RXDATAL;

    if (command_pointer == UINT8_MAX)
    {
        printf("\r\n");
        command[command_pointer++] = '\0';
        command_parse(parsed_command, command);
        command_execute(parsed_command);
        command_pointer = 0;
    }
    else if (next_char == '\r')
    {
        printf("\n\r");
        command[command_pointer++] = '\0';
        command_parse(parsed_command, command);
        command_execute(parsed_command);
        command_pointer = 0;
    }
    else if (next_char == BACKSPACE)
    {
        printf("%c", next_char);
        if (command_pointer > 0)
        {
            command_pointer--;
        }
    }
    else
    {
        printf("%c", next_char);
        command[command_pointer++] = next_char;
    }
}
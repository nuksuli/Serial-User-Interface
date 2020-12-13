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

//Function prototypes
void USART0_init(void);
void command_parse(char **parsed_command, char *command);
void command_execute(char **parsed_command);
static FILE USART_stream;

//Declaration of global variables
char *command;
char **parsed_command;
uint8_t command_pointer = 0;
uint8_t ch;

//Initialise serial data transfer
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

static int USART0_charprint(char c, FILE *stream)
{
    while (!(USART0.STATUS & USART_DREIF_bm))
    {
        ;
    }
    USART0.TXDATAL = c;
    return 0;
}
static FILE USART_stream = FDEV_SETUP_STREAM(USART0_charprint,
                                             NULL, _FDEV_SETUP_WRITE);

void PERIPHERAL_init(void)
{
    PORTF.DIRSET = PIN5_bm;
    PORTF.OUTSET = PIN5_bm;
    PORTF.DIRCLR = PIN6_bm;
}

//Parse arguments separated by spaces from input string
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

//Execute command defined by parsed arguments.
void command_execute(char **parsed_command)
{
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
        /*
        else if (strcmp(parsed_command[1], "SET") == 0)
        {
            pwm_period(atoi(parsed_command[2]));
        }
         */
        else
        {
            if (LED_status())
            {
                printf("LED is currently OFF\n\r");
            }
            else
            {
                printf("LED is currently ON\n\r");
            }
        }
    }
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
    else if (strcmp(parsed_command[0], "TEMP") == 0)
    {
        uint16_t temp = temperature();
        printf("Temperature: %d \n\r", temp);
    }
    else if (strcmp(parsed_command[0], "RESET") == 0)
    {
        printf("Resetting...\r\n");
        reset();
    }
    else if (strcmp(parsed_command[0], "ADC") == 0)
    {
        if (strcmp(parsed_command[1], "SET") == 0)
        {
            if (strstr(parsed_command[2], "AN") != NULL)
            {
                ch = *(parsed_command[2] + 2) - 48;
                if (strlen(parsed_command[2]) == 3)
                {
                    if (ADC0_set_channel(ch) == 1)
                    {
                        printf("Channel set to: %i \r\n", ch);
                    }
                    else
                    {
                        printf("Invalid channel\r\n");
                    }
                }
                else if (strlen(parsed_command[2]) == 4)
                {
                    ch = *(parsed_command[2] + 3) - 38;
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
        }
        else
        {
            uint16_t adc = ADC0_conversion();
            printf("Conversion result: %d \n\r", adc);
        }
    }
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
                   "\tADC SET AN<n>\t set analog input pin\n\r"
                   "\tAvailable pins: 0-15\n\r");
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
    for (uint8_t i = 0; i < UINT8_MAX; i++)
    {
        parsed_command[i] = "\0";
    }
}

int main(void)
{
    PERIPHERAL_init();
    USART0_init();
    command = malloc(UINT8_MAX * sizeof(char));
    parsed_command = malloc(5 * sizeof(char *));
    sei();

    printf("Program starting!\r\n");

    while (1)
    {
        ;
    }
}

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
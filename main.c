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
#include <stdio.h>
#include <string.h>
#include "temperature.h"
#include "reset.h"
#include "adc_conversion.h"
#include "vref.h"

//Function prototypes
void USART0_init(void);
void USART0_charsend(char c);
void USART0_send(char *str);
char USART0_charread(void);
void USART0_read(char *command);
void LED_on(void); 
void LED_off(void);
void LED_init(void);
void command_parse(char *parsed_command[], char *command);
void command_execute(char *parsed_command[]);
static FILE USART_stream;

//Initialize serial data transfer
void USART0_init(void)
{
    PORTA.DIRCLR = PIN1_bm;
    PORTA.DIRSET = PIN0_bm;
    
    USART0.BAUD = (uint16_t)USART0_BAUD_RATE(9600);

    USART0.CTRLB |= USART_RXEN_bm | USART_TXEN_bm;
    stdout = &USART_stream;
}

//Send a character to serial
void USART0_charsend(char c)
{
    while (!(USART0.STATUS & USART_DREIF_bm))
    {
        ;    
    }
    USART0.TXDATAL = c;
}

static int USART0_charprint(char c, FILE *stream)
{ 
    USART0_charsend(c);
    return 0; 
}
static FILE USART_stream = FDEV_SETUP_STREAM(USART0_charprint, 
        NULL, _FDEV_SETUP_WRITE);

//Send a string to serial
void USART0_send(char *str)
{
    for(size_t i = 0; i < strlen(str); i++)
    {
        USART0_charsend(str[i]);
    }
}

//Read a character from serial
char USART0_charread(void)
{
    while (!(USART0.STATUS & USART_RXCIF_bm))
    {
        ;
    }
    return USART0.RXDATAL;
}

//Read input string from serial. Terminated by ENTER.
//TODO: Follow cursor.
void USART0_read(char *command)
{
    uint8_t index = 0;
    while (index <= MAX_COMMAND_LEN)
    {
        char next_char = USART0_charread();
        //ENTER
        if (next_char == '\r')
        {
            USART0_send("\r\n");
            command[index] = '\0';
            return;
        }
        else if (next_char == BACKSPACE)
        {
            USART0_charsend(next_char);
            if (index > 0)
            {
                index--;
            }
        }
        else
        {
            USART0_charsend(next_char);
            command[index++] = next_char;
        }
    }
    USART0_send("\r\n");
    command[MAX_COMMAND_LEN + 1] = '\0';
}
//Turn on LED (PF5)
void LED_on(void)
{
    PORTF.OUTCLR = PIN5_bm;
}

//Turn off LED (PF5)
void LED_off(void)
{
    PORTF.OUTSET = PIN5_bm;
}

//Set LED to output and button to input.
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
void command_execute(char *parsed_command[])
{
    if(strcmp(parsed_command[0], "LED") == 0)
    {
        if (strcmp(parsed_command[1], "ON") == 0)
        {
            LED_on();
        }
        else if (strcmp(parsed_command[1], "OFF") == 0)
        {
            LED_off();
        }
        else
        {
            if (PORTF.OUT & PIN5_bm)
            {
                USART0_send("LED status: OFF\r\n");
            }
            else 
            {
                USART0_send("LED status: ON\r\n");
            }
        }
    }
    else if(strcmp(parsed_command[0], "TEMP") == 0)
    {
        uint16_t temp = temperature();
        printf("Temperature: %d \n\r", temp);
    }
    else if(strcmp(parsed_command[0], "RESET") == 0)
    {
        USART0_send("Resetting...\r\n");
        reset();
    }
    else if(strcmp(parsed_command[0], "ADC") == 0)
    {
        if(strcmp(parsed_command[1], "SET") == 0)
        {
            if(strstr(parsed_command[2], "AN") != NULL)
            {
                if(ADC0_set_channel(parsed_command[2 + 2]) == 1)
                {
                    printf("Channel set to: %i \r\n", parsed_command[2] + 2);
                }
                else
                {
                    printf("Invalid channel\r\n");
                }
            }
        }
        else
        {
            uint16_t adc = ADC0_conversion();
            printf("Conversion result: %d \n\r", adc);
        }
    }
    else if(strcmp(parsed_command[0], "VREF") == 0)
    {   
        if(strcmp(parsed_command[1], "SET") == 0)
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
        else if (parsed_command[1] != "\0")
        {
            printf("INVALID ARGUMENT\n\r");
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
    } else if (strcmp(parsed_command[0], "HELP") == 0) {
        if(strcmp(parsed_command[1], "LED") == 0) {
            USART0_send("Available LED commands:\n\tLED\t\t\t\t print LED driver status\n\tLED [ON|OFF]\t TURN LED on or off\n\tLED SET <n>\t\t set led brightness(0 <= n <= 255)");
        } else if(strcmp(parsed_command[1], "BTN") == 0) {     
            USART0_send("Available BTN commands:\n\tBTN\t\t\t\t print button status\n\tINV [ON|OFF]\t configure state invert\n\tPUP [ON|OFF]\t configure pull-up resistor");
        } else {
            USART0_send("Available commands:\n\tLED \tLED Settings (HELP LED for Details)\n\tBTN \tButton Settings (HELP BTN for Details)\n\tANSI\tDisplay settings\n\tHELP\tThis Help\n\tRESET\tReset the microcontroller");
        }
  
    }
    else 
    {
        USART0_send("NOT A VALID COMMAND!\r\n");
    }
    for (int i = 0; i < MAX_ARGUMENT_LEN; i++)
    {
        parsed_command[i] = "\0";
    }
}

int main(void)
{
    char command[MAX_COMMAND_LEN + 1];
    char parsed_command[MAX_ARGUMENT_LEN][MAX_COMMAND_LEN + 1];
    
    PERIPHERAL_init();
    USART0_init();
    USART0_send("Program starting! \r\n");
    
    while (1)
    {
        USART0_read(command);
        command_parse(parsed_command, command);
        command_execute(parsed_command);
    }
}

#define F_CPU 3333333
#define USART0_BAUD_RATE(BAUD_RATE) \
        ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)
#define MAX_COMMAND_LEN 8
#define BACKSPACE 127

#include <avr/io.h>
#include <stdio.h>
#include <string.h>

void USART0_init(void);
void USART0_charsend(char c);
void USART0_send(char *str);
char USART0_charread(void);
void USART0_read(char *command);
void LED_on(void);
void LED_off(void);
void LED_init(void);
void command_execute(char *command);

void USART0_init(void)
{
    PORTA.DIRCLR = PIN1_bm;
    PORTA.DIRSET = PIN0_bm;
    
    USART0.BAUD = (uint16_t)USART0_BAUD_RATE(9600);

    USART0.CTRLB |= USART_RXEN_bm | USART_TXEN_bm;
}

void USART0_charsend(char c)
{
    while (!(USART0.STATUS & USART_DREIF_bm))
    {
        ;    
    }
    USART0.TXDATAL = c;
}

void USART0_send(char *str)
{
    for(size_t i = 0; i < strlen(str); i++)
    {
        USART0_charsend(str[i]);
    }
}

char USART0_charread(void)
{
    while (!(USART0.STATUS & USART_RXCIF_bm))
    {
        ;
    }
    return USART0.RXDATAL;
}

void USART0_read(char *command)
{
    uint8_t index = 0;
    while (index < MAX_COMMAND_LEN + 1)
    {
        char next_char = USART0_charread();
        if (next_char == '\r')
        {
            USART0_send("\r\n");
            command[index] = '\0';
            return;
        }
        else if (next_char == BACKSPACE)
        {
            USART0_charsend(next_char);
            index--;
        }
        else if (index == MAX_COMMAND_LEN)
        {
            USART0_send("\r\n");
            command[index] = '\0';
            return;
        }
        else
        {
            USART0_charsend(next_char);
            command[index++] = next_char;
        }
    }
}

void LED_on(void)
{
    PORTB.OUTCLR = PIN5_bm;
}

void LED_off(void)
{
    PORTB.OUTSET = PIN5_bm;
}

void LED_init(void)
{
    PORTB.DIRSET = PIN5_bm;
}

void command_execute(char *command)
{
    if(strcmp(command, "LED") == 0)
    {
        if (PORTF.OUT & PIN6_bm)
        {
            USART0_send("LED status: ON\r\n");
        }
        else 
        {
            USART0_send("LED status: OFF\r\n");
        }
    }
    else 
    {
        USART0_send("NOT A VALID COMMAND!\r\n");
    }
}

int main(void)
{
    char command[MAX_COMMAND_LEN];
    
    LED_init();
    USART0_init();
    USART0_send("Program starting! \r\n");
    
    while (1)
    {
        USART0_read(command);
        command_execute(command);
    }
}

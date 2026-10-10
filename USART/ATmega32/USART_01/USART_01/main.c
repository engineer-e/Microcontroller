/*Your program is an ATmega32 UART relay-control system. It receives commands from a serial terminal, compares them with strings such as `rl1On` and `allOff`, and controls four relays connected to `PC0`–`PC3`.

The original code has several problems: the `#include <string.h>` directive is missing, the `strcmp()` comparisons need a properly terminated string, the receive buffer can overflow, and the Enter-key check can run before a new character arrives.

Here is the corrected complete code for ATmega32, 16 MHz, and 9600 baud.

c*/


/*
 * uart_example_2.c
 *
 * ATmega32 UART Relay Control
 * Clock: 16 MHz
 * Baud rate: 9600
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#define BAUD_RATE 9600UL
#define uartReady (UCSRA & (1 << RXC))

#define BUFFER_SIZE 16

char txt[BUFFER_SIZE];
unsigned char i = 0;

// Initialize UART
void uartInit(unsigned long baud)
{
    unsigned int ubrr;

    ubrr = (F_CPU / (16UL * baud)) - 1;

    UBRRH = (unsigned char)(ubrr >> 8);
    UBRRL = (unsigned char)ubrr;

    // Enable transmitter and receiver
    UCSRB = (1 << RXEN) | (1 << TXEN);

    // Asynchronous, 8 data bits, no parity, 1 stop bit
    UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
}

// Transmit one character
void uartTransmit(unsigned char data)
{
    while (!(UCSRA & (1 << UDRE)))
    {
    }

    UDR = data;
}

// Receive one character
unsigned char uartReceive(void)
{
    while (!(UCSRA & (1 << RXC)))
    {
    }

    return UDR;
}

// Transmit a string
void sendText(const char *txt)
{
    while (*txt != '\0')
    {
        uartTransmit((unsigned char)*txt);
        txt++;
    }
}

// Clear the receive buffer
void clearAll(void)
{
    i = 0;
    txt[0] = '\0';
}

// Process received command
void processCommand(void)
{
    // End the string before comparing
    txt[i] = '\0';

    // Relay 1 control
    if (strcmp(txt, "rl1On") == 0)
    {
        PORTC |= (1 << PC0);
        sendText("Relay 1 Turns On.\r\n");
    }
    else if (strcmp(txt, "rl1Off") == 0)
    {
        PORTC &= ~(1 << PC0);
        sendText("Relay 1 Turns Off.\r\n");
    }

    // Relay 2 control
    else if (strcmp(txt, "rl2On") == 0)
    {
        PORTC |= (1 << PC1);
        sendText("Relay 2 Turns On.\r\n");
    }
    else if (strcmp(txt, "rl2Off") == 0)
    {
        PORTC &= ~(1 << PC1);
        sendText("Relay 2 Turns Off.\r\n");
    }

    // Relay 3 control
    else if (strcmp(txt, "rl3On") == 0)
    {
        PORTC |= (1 << PC2);
        sendText("Relay 3 Turns On.\r\n");
    }
    else if (strcmp(txt, "rl3Off") == 0)
    {
        PORTC &= ~(1 << PC2);
        sendText("Relay 3 Turns Off.\r\n");
    }

    // Relay 4 control
    else if (strcmp(txt, "rl4On") == 0)
    {
        PORTC |= (1 << PC3);
        sendText("Relay 4 Turns On.\r\n");
    }
    else if (strcmp(txt, "rl4Off") == 0)
    {
        PORTC &= ~(1 << PC3);
        sendText("Relay 4 Turns Off.\r\n");
    }

    // All relays ON
    else if (strcmp(txt, "allOn") == 0)
    {
        PORTC |= 0x0F;
        sendText("All relays turn on.\r\n");
    }

    // All relays OFF
    else if (strcmp(txt, "allOff") == 0)
    {
        PORTC &= 0xF0;
        sendText("All relays turn off.\r\n");
    }

    else
    {
        sendText("Invalid command.\r\n");
    }

    // Prepare for the next command
    clearAll();
}

int main(void)
{
    unsigned char tmp;

    // Configure PC0-PC3 as outputs
    DDRC |= 0x0F;

    // Initially turn all four relays off
    PORTC &= 0xF0;

    // Initialize UART
    uartInit(BAUD_RATE);

    sendText("ATmega32 UART Relay Control Ready.\r\n");
    sendText("Enter a command and press Enter.\r\n");

    while (1)
    {
        if (uartReady)
        {
            tmp = uartReceive();

            // Echo received character to terminal
            uartTransmit(tmp);

            // Process command on carriage return
            if (tmp == '\r')
            {
                processCommand();
            }
            // Ignore line feed from terminals sending CR+LF
            else if (tmp == '\n')
            {
                // Do nothing
            }
            // Store ordinary characters safely
            else if (i < BUFFER_SIZE - 1)
            {
                txt[i++] = (char)tmp;
            }
            else
            {
                sendText("\r\nCommand too long.\r\n");
                clearAll();
            }
        }
    }
}

/*
## Commands to enter in the serial terminal

| Command | Action |
| --- | --- |
| `rl1On` | Turn ON relay 1 (PC0) |
| `rl1Off` | Turn OFF relay 1 (PC0) |
| `rl2On` | Turn ON relay 2 (PC1) |
| `rl2Off` | Turn OFF relay 2 (PC1) |
| `rl3On` | Turn ON relay 3 (PC2) |
| `rl3Off` | Turn OFF relay 3 (PC2) |
| `rl4On` | Turn ON relay 4 (PC3) |
| `rl4Off` | Turn OFF relay 4 (PC3) |
| `allOn` | Turn ON all four relays |
| `allOff` | Turn OFF all four relays |

## Important corrections explained

1. Added `<string.h>` because `strcmp()` is declared in this header.
2. Fixed string termination: `txt[i] = '\0'` ensures `strcmp()` reads a valid string.
3. Fixed buffer overflow: the program limits the command length to 15 characters.
4. Fixed Enter-key handling: commands are processed only when a carriage return is received.
5. Handled CR and LF: terminals may send both characters when Enter is pressed, so `'\n'` is ignored.
6. Preserved other `PORTC` pins: bitwise operations control PC0–PC3 without unnecessarily changing PC4–PC7.
7. Simplified command comparisons: the stored text excludes the Enter character, so commands are compared against `"rl1On"` rather than `"rl1On\r"`.

Hardware note: Connect the ATmega32 UART pins `PD1/TXD` and `PD0/RXD` to the serial terminal's RXD and TXD respectively. For physical relays, use a suitable transistor driver or relay-driver IC rather than driving relay coils directly from the microcontroller pins.
*/

#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>

/* LCD data pins */
#define LCD_DATA_PORT PORTB
#define LCD_DATA_DDR  DDRB
#define LCD_DATA_PIN  PINB

/* LCD control pins */
#define LCD_CTRL_PORT PORTD
#define LCD_CTRL_DDR  DDRD

#define EN PD0
#define RW PD1
#define RS PD2

/* Function declarations */
void LCD_Enable(void);
void LCD_CheckBusy(void);
void LCD_Command(unsigned char command);
void LCD_Character(unsigned char character);
void LCD_String(const char *string);
void LCD_Scroll(void);

int main(void)
{
    /* Set control pins as output */
    LCD_CTRL_DDR |= (1 << EN) |
                    (1 << RW) |
                    (1 << RS);

    /* Set data pins as output */
    LCD_DATA_DDR = 0xFF;

    /* LCD power-up delay */
    _delay_ms(20);

    /* LCD initialization */
    LCD_Command(0x38);     // 8-bit mode, 2 lines, 5x8 font
    LCD_Command(0x08);     // Display OFF
    LCD_Command(0x01);     // Clear display
    _delay_ms(2);

    LCD_Command(0x06);     // Cursor moves right
    LCD_Command(0x0C);     // Display ON, cursor OFF

    /* First line */
    LCD_String("Microchip (^o^)");

    /* Move cursor to second line */
    LCD_Command(0xC0);

    /* Second line */
    LCD_String("Gobal Krishnan V");

    /* Scrolling animation */
    while (1)
    {
        LCD_Scroll();
    }
}

/*
 * Generate Enable pulse
 */
void LCD_Enable(void)
{
    LCD_CTRL_PORT |= (1 << EN);

    _delay_us(1);

    LCD_CTRL_PORT &= ~(1 << EN);

    _delay_us(1);
}

/*
 * Check LCD Busy Flag
 */
void LCD_CheckBusy(void)
{
    unsigned char status;

    /* Set data bus as input */
    LCD_DATA_DDR = 0x00;

    /* RW = 1 -> Read */
    LCD_CTRL_PORT |= (1 << RW);

    /* RS = 0 -> Command */
    LCD_CTRL_PORT &= ~(1 << RS);

    do
    {
        /* Enable HIGH */
        LCD_CTRL_PORT |= (1 << EN);

        _delay_us(1);

        /* Read status from LCD */
        status = LCD_DATA_PIN;

        /* Enable LOW */
        LCD_CTRL_PORT &= ~(1 << EN);

        _delay_us(1);

    } while (status & 0x80);   // Check D7 busy flag

    /* Data bus back to output */
    LCD_DATA_DDR = 0xFF;

    /* RW = 0 -> Write */
    LCD_CTRL_PORT &= ~(1 << RW);
}

/*
 * Send command to LCD
 */
void LCD_Command(unsigned char command)
{
    LCD_CheckBusy();

    /* RS = 0 -> Command */
    LCD_CTRL_PORT &= ~(1 << RS);

    /* RW = 0 -> Write */
    LCD_CTRL_PORT &= ~(1 << RW);

    /* Put command on data bus */
    LCD_DATA_PORT = command;

    /* Enable pulse */
    LCD_Enable();

    /* Clear data bus */
    LCD_DATA_PORT = 0x00;
}

/*
 * Send character to LCD
 */
void LCD_Character(unsigned char character)
{
    LCD_CheckBusy();

    /* RS = 1 -> Data */
    LCD_CTRL_PORT |= (1 << RS);

    /* RW = 0 -> Write */
    LCD_CTRL_PORT &= ~(1 << RW);

    /* Put character on data bus */
    LCD_DATA_PORT = character;

    /* Enable pulse */
    LCD_Enable();

    /* Clear data bus */
    LCD_DATA_PORT = 0x00;
}

/*
 * Send string to LCD
 */
void LCD_String(const char *string)
{
    while (*string != '\0')
    {
        LCD_Character(*string);
        string++;
    }
}

/*
 * Scrolling animation
 *
 * 0x18 = Shift display left
 * 0x1C = Shift display right
 */
void LCD_Scroll(void)
{
    unsigned char i;

    /* Scroll left */
    for (i = 0; i < 16; i++)
    {
        LCD_Command(0x18);

        _delay_ms(300);
    }

    /* Scroll right */
    for (i = 0; i < 16; i++)
    {
        LCD_Command(0x1C);

        _delay_ms(300);
    }
}
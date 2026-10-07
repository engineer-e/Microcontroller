/*
 * main.c
 *
 * Created: 10/8/2026 12:20:14 AM
 *  Author: Gobal Krishnan V
 */ 

#include <xc.h>

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRB |= 1 << PINB0;
	DDRB &= ~(1 << PINB1);
	PORTB |= 1 << PINB1;
	while (1)
	{
		PORTB ^= 1 << PINB0;
		if (bit_is_clear(PINB, 1))
		{
			_delay_ms(10); //Fast
		}
		else
		{
			_delay_ms(100); //Slow, from previous
		}
	}
}
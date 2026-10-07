/*
 * main.c
 *
 * Created: 10/8/2026 12:38:33 AM
 *  Author: Gobal Krishnan V
 */ 

#include <xc.h>

#include <avr/io.h>

int main(void)
{
	//Initialize port for LEDs
	DDRB = 0b00000001;
	PORTB = 0b00000000;
	TCCR1B |= 1<<CS10;
	
	int repeatCount = 0;
	
	while(1)
	{
		if (TCNT1 > 10000)
		{
			repeatCount++;
			TCNT1 = 0;
			if (repeatCount > 100)
			{
				repeatCount = 0;
				PORTB ^= 1 << PINB0;
			}
		}
		// Count and turn an LED ON and OFF
	}
}
/*
 * main.c
 *
 * Created: 10/7/2026 10:58:40 PM
 *  Author: Gobal Krishnan V
 */ 

#include <xc.h>

#include <avr/io.h>

#ifndef F_CPU
#define F_CPU 16000000UL  // Define clock speed as 16 MHz
#endif

#include <util/delay.h>



int main(void)
{
  DDRB |= 1 << PINB0;
  while (1)
  {
    PORTB |= 1 << PINB0;
    _delay_ms(1000);
    PORTB &= ~(1 << PINB0);
    _delay_ms(1000);
  }
}
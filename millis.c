/***********************************************
Millisecond timer
***********************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/power.h>
#include <util/atomic.h>
#include "millis.h"

#define F_CPU 12000000UL

volatile long timer_millis;

void millis_init(void)
{
	
	/*
	* Run timer 1 in CTC mode, prescaler 8
	*/
	TCCR1B = _BV(WGM12) | _BV(CS11);
  
	/*
	* Compare match after 1ms
	*/
	OCR1A = (F_CPU/8*.001)-1; //1499
  
	/*
	* Compare match 1A interrupts.
	*/
	TIMSK |= _BV(OCIE1A);

	// enable interrupts
	sei();

}

long millis_get()
{
	long millis_return;
	
	// Ensure this cannot be disrupted
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		millis_return = timer_millis;
	}
	
	return millis_return;
}

void millis_reset()
{
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		timer_millis=0;
	}
	
}

void millis_add(long ms)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		timer_millis+=ms;
	}
}


void millis_subtract(long ms)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		timer_millis-=ms;
	}
}

void millis_resume()
{
	millis_init();
}

void millis_pause()
{
	//TCCR1B=0;
}

ISR (TIMER1_COMPA_vect)
{
	// action to be done every 1ms
	timer_millis++;
}
/*
 * BinaryClockAVR.c
 *
 * Created: 7/16/2011 1:19:31 PM
 *  Author: Vlad
 */ 
#define F_CPU 1500000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define COL_A 3
#define COL_B 4
#define COL_C 5	
#define ROW_1 0
#define ROW_2 1
#define ROW_3 2
#define ROW_4 3
#define ROW_5 5
#define ROW_6 4
#define BTN_1 6
#define BTN_2 7



uint16_t ms_10 = 0; //keeps track of 1s/300 periods
uint8_t seconds = 0;
uint8_t minutes = 0;
uint8_t hours = 0;
unsigned char column = 0;



void init_timer()
{
	TCCR1B |= (1<<WGM12)|(1<<WGM13);//configure timer1 for CTC 
	TIMSK |= (1<<OCIE1A); //set interrupt to compare mode
	sei(); //activate global interrupts
	ICR1 = 624; //set top limit of counter (ticks every 1/300 second)
	TCCR1B |= (1<<CS11);  //start timer with prescaler = 8
}

unsigned char check_input()
{
	if((PINB | ~(1<<BTN_1)) == ~(1<<BTN_1))
	{
		_delay_ms(25);
		if((PINB | ~(1<<BTN_1)) == ~(1<<BTN_1)) return 1;
	}

	if((PINB | ~(1<<BTN_2)) == ~(1<<BTN_2))
	{
		_delay_ms(25);
		if((PINB | ~(1<<BTN_2)) == ~(1<<BTN_2)) return 2;
	}
	return 0;
}

void output(unsigned char val)
{
	if((val & (1<<0)) == (1<<0))
		PORTB |= (1<<ROW_1);
	
	if((val & (1<<1)) == (1<<1))
		PORTB |= (1<<ROW_2);
	
	if((val & (1<<2)) == (1<<2))
		PORTB |= (1<<ROW_3);
	
	if((val & (1<<3)) == (1<<3))
		PORTB |= (1<<ROW_4);
	
	if((val & (1<<4)) == (1<<4))
		PORTB |= (1<<ROW_5);
	
	if((val & (1<<5)) == (1<<5))
		PORTB |= (1<<ROW_6);	
}

int main(void)
{
	unsigned char btn_test;
	DDRB = 0b00111111; //set up port B
	DDRD |= (1<<COL_A)|(1<<COL_B)|(1<<COL_C);
	init_timer();	
	
    while(1)
    {
		
		
		btn_test = check_input();
		if(btn_test == 1)
			minutes++;
		else if(btn_test == 2)
			hours++;
		_delay_ms(200);		
    } 
}

ISR(TIMER1_COMPA_vect)
{
	ms_10++;
	if (ms_10 >= 300)
	{
		seconds++;
		ms_10 = 0;
	}		
	if (seconds >= 60)
	{
		minutes++;
		seconds = 0;
	}			
	if (minutes >= 60)
	{
		hours++;
		minutes = 0;
	}
	if (hours >= 24)
	{
		hours = 0;
	}
	
	//Turn off everything
	PORTB &= ~(1<<ROW_1 | 1<<ROW_2 | 1<<ROW_3 | 1<<ROW_4 | 1<<ROW_5 | 1<<ROW_6);
	PORTD |= (1<<COL_A | 1<<COL_B | 1<<COL_C);
	
	if((PIND & (1<<6)) == (1<<6))
	{
		column++;
		if (column >= 3) column = 0;
		
	
	
		switch (column)
		{
			case 0:
				PORTD &= ~(1<<COL_A);
				output(hours);
				break;
			case 1:
				PORTD &= ~(1<<COL_B);
				output(minutes);
				break;
			case 2:
				PORTD &= ~(1<<COL_C);
				output(seconds);
				break;
			default:
				column = 0;
				break;
		}
	}	
	
}
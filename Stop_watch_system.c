/*
 * main.c
 *
 *  Created on: Jan 22, 2022
 *      Author: SAYED IBRAHIM
 */



#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>



unsigned char Seconds = 0;
unsigned char Minutes = 0;
unsigned char Hours   = 0;



void INT0_Init(void) /*  external interrupt0 function */
{
	DDRD  &= ~(1<<PD2)  ;  /* choose the direction of external interrupt pin(PD2) and make it as input  */
	PORTD |=  (1<<PD2)  ;  /* activate internal pull up resistor  (PD2)  */
	GICR  |=  (1<<INT0) ;  /* choose the interrupt pin on register GICR  */
	MCUCR |=  (1<<ISC01);
	MCUCR &= ~(1<<ISC00);  /* falling edge */
}
void INT1_Init(void)       /* external interrupt1 function */
{
	DDRD  &= ~(1<<PD3); /*choose the direction of external interrupt pin(PD3)
 	 a make it as input and using  external pull down resistor*/
	GICR  |=  (1<<INT1);   /*choose the interrupt pin on register GICR*/
	MCUCR |=  (1<<ISC10) | (1<<ISC11);/*Rising edge*/
}
void INT2_Init(void) /* external interrupt2 function*/
{
	DDRB   &= ~(1<<PB2);  /*choose the direction of external interrupt pin(PB2) and make it as input*/
	PORTB  |=  (1<<PB2);     /* high in pin (PB2)*/
	GICR   |=  (1<<INT2);    /*choose the interrupt pin on register GICR*/
	MCUCSR &= ~(1<<ISC2);    /*falling edge*/

}
void Timer1_CTC_Mode(unsigned short tick)
{
	TCNT1   = 0;
	OCR1A   = tick;
	TIMSK  |= (1<<OCIE1A);
	TCCR1A |= (1<<FOC1A );
	TCCR1B  = (1<<WGM12)|(1<<CS10)|(1<<CS11);
}

ISR(INT0_vect)
{
	if(! ( PIND &(1<<PD2) ) )/*Check for internal pull up Reset flag*/
	{
		/*  by assign all varibles with zero */
		Seconds = 0;
		Minutes = 0;
		Hours   = 0;
	}
}
ISR(INT1_vect)
{
	if(PIND &(1<<PD3))/*Check for external pull down pause flag*/
	{
		TCCR1B &= ~(1<<CS12) & ~(1<<CS11) & ~(1<<CS10); /* Stop the clock */
	}
}
ISR(INT2_vect)
{
	if( !( PINB &(1<<PB2) ) )/*Check for internal pull up Resume flag*/
	{
		TCCR1B |= (1<<CS11)| (1<<CS10);   /* re initiate the clock */
	}
}
ISR(TIMER1_COMPA_vect)
{
	++Seconds;
	if(Seconds==60)
	{
		++Minutes;
		Seconds=0;
	}
	if(Minutes==60)
	{
		Minutes=0;
		++Hours;
	}
}
int main()
{
	DDRC  |= 0x0F;   /* O/p directions for 4 bins(decoder)					 */
	DDRA  |= 0x3F;   /* O/p directions for 6 bins(six 7Segment)				 */
	PORTC &= 0xF0;   /* PORTC = PORTC & 1111 0000 O/p 4 bins(decoder) is low */
	PORTA |= 0x3F;   /* The 6 bins(six 7Segment) is high 					 */


	Timer1_CTC_Mode(15624);

	INT0_Init();  /* initialize external interrupt0 by calling her function  */
	INT1_Init();  /* initialize external interrupt1 by calling her function  */
	INT2_Init();  /* initialize external interrupt2 by calling her function  */

	SREG  |= (1<<7);/*enable I-bit*/
	while(1)
	{
		PORTA= (PORTA & 0xC0) | 0x01; /*On digit one*/
		PORTC= (PORTC & 0xF0) | ( Seconds %10);/*count the seconds in first 7-seg*/
		_delay_us(3);
		PORTA= (PORTA & 0xC0) | 0x02; /*On digit two*/
		PORTC= (PORTC & 0xF0) | ( Seconds /10);/*count the seconds in second 7-seg*/
		_delay_us(3);

		PORTA= (PORTA & 0xC0) | 0x04; /*On digit three*/
		PORTC= (PORTC & 0xF0) | ( Minutes %10);/*count the minutes in third 7-seg*/
		_delay_us(3);
		PORTA= (PORTA & 0xC0) | 0x08; /*On digit four*/
		PORTC= (PORTC & 0xF0) | ( Minutes /10);/*count the minutes in the fourth 7-seg*/
		_delay_us(3);

		PORTA= (PORTA & 0xC0) | 0x10; /*On digit five*/
		PORTC= (PORTC & 0xF0) | ( Hours %10);   /* count the hours in fifth 7-seg*/
		_delay_us(3); 
		PORTA= (PORTA & 0xC0) | 0x20; /*On digit six*/
		PORTC= (PORTC & 0xF0) | ( Hours /10);   /* count the hours in sixth 7-seg*/
		_delay_us(3);
	}
}

/*	 Name: main.c
 *	 Author: <insert your name here>
 *	 Copyright: <insert your copyright message here>
 *	 License: <insert your license reference here>		 */


//#include <stdio.h>



#include <avr/io.h>
#include "m_general.h"
#include "m_bus.h"
#include "m_rf.h"
#include "m_usb.h"
#include <math.h> 
#include <avr/interrupt.h>

#define 	PI		3.14159265
#define 	FREQ 		370 		// Middle of 300-440Hz range 
#define 	SYSCLK		16000000	// Defauld sysclk val 16MHz
#define 	CLKDIV 		0   		// required 16MHz for m_bus to work
#define 	RES    		4398		// Length of array holding full sin wav.
#define 	CLKPERIOD	1000		// OCR1A

int sinVals[ RES ];
int period;
volatile int sinValsIndex;
volatile int next;

int m_NOF(int);
void setupVals(void);
void setup_timer(void);


int main(void)
{
	m_bus_init();
	m_clockdivide( CLKDIV );
	m_green(ON);
//	period = (long)((double)SYSCLK/((long)FREQ * RES ));	// set clock ticks/val 	
	set(DDRC,6);
	set(PORTC,6);	
	int C6Pin;
	C6Pin=1;
	int hRES=RES/2;
	period=400; 
	next=0;
	setupVals();
	setup_timer();
	int j;
	j=1;
	while(j){
		if (next==1){
			OCR1B=sinVals[ sinValsIndex  ];
			sinValsIndex++;
			if( sinValsIndex==RES  ){
				sinValsIndex=0;		
			}
		next=0;
		}
	}
/*
	int j;
	for (j=0 ; j < RES ; j++){
		printf("%d \t %d \n" , j , sinVals[j] );
	}		
	printf("period: \t %d",period);
*/
	return 0;   /* never reached */
}

void setupVals(){
	int i;
	for ( i=0 ; i < RES ; i++){
		sinVals[i] = ( int )(( cos( ((double)i * 2 * PI) / ( RES - 1 ) )/2.0+.5) * period );
	//	sinVals[i] = (int)( (double) period / (double) RES * i );
}	
	
}
void  setup_timer(){
	sinValsIndex=1;	
	OCR1A=period + 2;				// Set OCR1A to be adjusted for the frequency. 16MHz/(370hz*RES*clkdiv)
	OCR1B=20;

	// Setting up clock divider		
	clear(TCCR1B,CS12);	//1,0,1 provides a 1/16 divider for the 16MHz clock -> <1MHz
	clear(TCCR1B,CS11);
	set(TCCR1B, CS10);	
	// Set mode15 on timer 1
	set(TCCR1B,WGM13);
	set(TCCR1B,WGM12);
	set(TCCR1A,WGM11);
	set(TCCR1A,WGM10);
	// Controll B6 with the timer
	set(DDRB,6);
	// Match b/w TCNT1 and OCR1B clears OCR1B, set at rollover
	set(TCCR1A,COM1B1);
	clear(TCCR1A,COM1B0);	
	// set(TIFR1,TOV1);
	set(TIMSK1,OCIE1A);	// call an interupt when timer 1 rolls over
//	set(TIMSK1,OCIE1B);	
	sei(); 		//enable global interupts
}

ISR( TIMER1_COMPA_vect ){
	next=1;
}

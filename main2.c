/*	 Name: main.c
 *	 Author: <insert your name here>
 *	 Copyright: <insert your copyright message here>
 *	 License: <insert your license reference here>		 */


//#include <stdio.h>


#include <avr/io.h>
#include "m_general.h"
#include "m_bus.h"
#include "m_usb.h"
#include <math.h> 
#include <avr/interrupt.h>

#define 	PI		3.14159265
#define 	FREQ 		370 		// Middle of 300-440Hz range 
#define 	SYSCLK		16000000	// Defauld sysclk val 16MHz
#define 	CLKDIV 		0   		// required 16MHz for m_bus to work
#define 	RES    		100 		// Length of array holding full sin wav.
#define 	CLKPERIOD	1000		// OCR1A

int sinVals[ RES ];
int period;
void setupVals(void);
void setup_timer(void);
volatile int sinValsIndex;
int main(void)
{
	//m_bus_init();
	m_clockdivide( CLKDIV );
	m_green(ON);
	period = (long)((double)SYSCLK/((long)FREQ * RES ));	// set clock ticks/val 	
	
	setupVals();

	int j;
	j=0;
	while(fabs(j) > -1){
	j++;
	}

//	int j;
//	for (j=0 ; j < RES ; j++){
//		printf("%d \t %d \n" , j , sinVals[j] );
//	}		
//	printf("period: \t %d",period);

	return 0;   /* never reached */
}

void setupVals(){
	int i;
	for ( i=0 ; i < RES ; i++){
		sinVals[i] = ( int )(fabs( cos( ((double)i * 2 * PI) / ( RES - 1 ) ) * period));
	//	sinVals[i] = (int)( (double) period / (double) RES * i );
}	
	
}
void  setup_timer(){
	sinValsIndex=1;	
	OCR1A=period;		// Set OCR1A to be adjusted for the frequency. 16MHz/(370hz*RES*clkdiv)
	OCR1B=10;		//CAUTION!!!!!!! not sure how timer will respond to OCR1B=0

	// Setting up clock divider		
	set(TCCR1B,CS12);	//1,0,1 provides a 1/8 divider for the 16MHz clock -> <125kHz
	set(TCCR1B,CS11);
	clear(TCCR1B, CS10);	
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
	set(TIMSK1,OCIE1B);	
	sei(); 		//enable global interupts
}

ISR(TIMER1_COMPA_vect){
	
	OCR1B=sinVals[sinValsIndex];
	sinValsIndex=sinValsIndex+1;
	if (sinValsIndex==(RES-1)){
		sinValsIndex=1;
	}
	m_red(ON);
}
ISR( TIMER1_COMPB_vect ){
	m_red( OFF );

}




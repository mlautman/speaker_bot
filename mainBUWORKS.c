#include <avr/io.h>
#include "m_general.h"
#include "m_bus.h"
#include "m_rf.h"
#include "m_usb.h"
#include <math.h> 
#include <avr/interrupt.h>

#define 	PI		3.14159265
#define 	FREQ 		400 		// Middle of 300-440Hz range 
#define 	SYSCLK		16000000	// Defauld sysclk val 16MHz
#define 	CLKDIV 		0   		// required 16MHz for m_bus to work
#define 	RES    		100		// Length of array holding full sin wav.
#define 	CLKPERIOD	1000		// OCR1A
int sinVals[ RES ];
int period;
volatile int sinValsIndex;
volatile int next;
void setupVals(void);
void setup_timer(void);
int main(void) {
//	m_bus_init();
	m_clockdivide( CLKDIV );
	m_green(ON);
	period = (long)((double)SYSCLK/((long)FREQ * RES ));	// set clock ticks/val 	
	next=0;
	setupVals();
	setup_timer();
	while(1){
		if (next==1){
			OCR1B=sinVals[ sinValsIndex  ];
			sinValsIndex++;
			if( sinValsIndex==RES  ){ sinValsIndex=0;}
		next=0; }
	} return 0;   /* never reached */
}
void setupVals(){
	int i; for ( i=0 ; i < RES ; i++){
		// initialize a lookup array of values for OCR1B that form  a sin wave b/w 0 and OCR1A
		sinVals[i] = ( int )(( cos( ((double)i * 2.0 * PI) / (double)( RES - 1 ) )/2.0+.5) * period ); 
	}	
}
void  setup_timer(){
	sinValsIndex=1;	
	OCR1A=period;		// Set OCR1A to be adjusted for the frequency. 16MHz/(370hz*RES*clkdiv)
	OCR1B=0;
	// Setting up clock divider		
	clear(TCCR1B,CS12); clear(TCCR1B,CS11); set(TCCR1B, CS10);	
	// Set mode15 on timer 1
	set(TCCR1B,WGM13); set(TCCR1B,WGM12); set(TCCR1A,WGM11); set(TCCR1A,WGM10);
	// Controll B6 with the timer
	set(DDRB,6);
	// Match b/w TCNT1 and OCR1B clears OCR1B, set at rollover
	set(TCCR1A,COM1B1); clear(TCCR1A,COM1B0);	
	// set(TIFR1,TOV1);
	set(TIMSK1,OCIE1A);	// call an interupt when timer 1 rolls over
//	set(TIMSK1,OCIE1B);	
	sei(); 		//enable global interupts
}
ISR( TIMER1_COMPA_vect ){ next=1; }

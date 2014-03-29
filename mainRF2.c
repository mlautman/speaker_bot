/*	 Name: main.c
 *	 Author: <insert your name here>
 *	 Copyright: <insert your copyright message here>
 *	 License: <insert your license reference here>		 */
#include <avr/io.h>
#include "m_general.h"
#include "m_bus.h"
#include "m_rf.h"
#include "m_usb.h"
#include <math.h> 
#include <avr/interrupt.h>

#define 	PI		3.14159265
#define 	FREQ_LO 	300 	
#define 	FREQ_HI		440
#define 	SYSCLK		16000000	// Defauld sysclk val 16MHz
#define 	CLKDIV 		0   		// required 16MHz for m_bus to work
#define 	RES    		100		// Length of array holding full sin wav.
#define 	CLKPERIOD	1000		// OCR1A
#define 	PACKET_LENGTH	3
#define 	CHANNEL		1
#define 	RXADDRESS	26
#define 	RANGE_SIZE 	FREQ_HI-FREQ_LO+1

volatile bool 	newPacket;
int 		currFreq;
int 		sinVals[RANGE_SIZE][ RES ];
volatile bool 	sinValsIndex;
volatile bool	next;
long	 	cycles;
char 		packet[3]={ 0,0,0 };
int 		FREQ;

void playSound(void);
void setupVals(void);
void setup_timer(void);
void m_rf_init(void);
void run_test(void);

int main(void)
{
	m_bus_init();		// initialize bus shit
	m_rf_init();		//
	m_clockdivide( CLKDIV );
	m_red(ON);
	next=newPacket=0;
	setupVals();
	setup_timer();

	run_test();
	
	while(1){
		if (cycles>0 &&  next==1){
			OCR1B=sinVals[ FREQ - FREQ_LO ][ sinValsIndex ];
			sinValsIndex++;
			if( sinValsIndex==RES  ){
				sinValsIndex=0;		
				cycles--;
			}
		next=0;
		}else if (cycles == 0)
			m_green(OFF);

		if ( newPacket ){
			newPacket = 0;
			m_green(ON);
			playSound();		
		}
	}
	return 0;   /* never reached */
}

void setupVals(){
	int i;
	int ocr1a;
	int j;
	int J;
	for ( i=0 ; i < RES ; i++){
		for ( j=0; j < RANGE_SIZE ; j++ ){
			J=j+FREQ_LO;
			ocr1a=(int)(16000000.0/(8.0* (float)(RES * J)));
			sinVals[j][i] = ( int )(( cos( ((double)i * 2 * PI) / ( RES - 1 ) )/2.0+.5) * ocr1a );
		}
	}
}	
	

void  setup_timer(){
	sinValsIndex=1;	
	OCR1A= 10000;				// Set OCR1A to be adjusted for the frequency. 16MHz/(370hz*RES*clkdiv)
	OCR1B=0;
	// Setting up clock divider		
	clear(TCCR1B,CS12);	//1,0,1 provides a 1/16 divider for the 16MHz clock -> <1MHz
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
//	set(TIMSK1,OCIE1B);	
	sei(); 		//enable global interupts
}


void m_rf_init(){
//	m_rf_open( CHANNEL, RXADDRESS, PACKET_LENGTH );
}
void playSound(){
//	FREQ=(packet[0] + packet[1]*16*16)/10;
//	cycles= (packet[2]*FREQ)/10;
	FREQ=300;
	cycles=3000;
	OCR1A=(int)(16000000.0/(8.0* (float)FREQ));

}
ISR( TIMER1_COMPA_vect ){
	next=1;
}

ISR( INT2_vect)
{
	m_green(ON);
//	m_rf_read(packet, PACKET_LENGTH);
	newPacket=1;
}

void run_test(){
	newPacket=1;
	m_green(ON);
	
}



#include <avr/io.h>
#include "m_general.h"
//#include "m_bus.h"
#include "m_rf.h"
#include "m_usb.h"
#include <math.h> 
#include <avr/interrupt.h>

#define 	PI		3.14159265
//#define 	FREQ 		400 		// Middle of 300-440Hz range 
#define 	SYSCLK		16000000			// Defauld sysclk val 16MHz
#define 	CLKDIV 		0   		// required 16MHz for m_bus to work
#define 	RES    		40		// Length of array holding full sin wav.
#define 	CLKPERIOD	1000		// OCR1A
#define 	PACKET_LENGTH	3
#define 	CHANNEL		1
#define		RXADDRESS	0x26


unsigned long FREQ;
bool newPacket;
unsigned long sinVals[ RES ];
int cycles ;
unsigned long period;
int sinValsIndex;
volatile int next;
char packet[3]={0,0,0};
int* feq_ptr=(int*)&packet[0];

void runTest();
void playSound(void);
void m_rf_init(void);
void setupVals(void);
void setup_timer(void);

int main(void) {

	int frequencyTest = 3300;
	char* ftestpt = (char*)&frequencyTest;
	char Fl = *(char*)ftestpt;
	char Fh = Fl+1;	
	char durrr = 0xFF;
	packet[0]=Fl;
	packet[1]=Fh;
	packet[2]=durrr;
	
//	m_usb_init()
	m_bus_init();
	m_rf_init();
	m_clockdivide( CLKDIV );
	period = (unsigned long)(((double)SYSCLK/( RES )));
//	next=0;
	setupVals();
	setup_timer();
	runTest();
	while(1){
		if (newPacket){	
			cli();
			playSound();
			sei();
			newPacket=0;
		}
		if (cycles>0){
			if (next==1){
				cli();
				OCR1B=sinVals[ sinValsIndex  ]/FREQ;
				sinValsIndex++;
				if( sinValsIndex==RES  ){
					 sinValsIndex=0;
					 cycles--;
				}
				sei();
				next=0; 
			}
		}else {m_green(OFF);}
	} return 0;   /* never reached */
}
void setupVals(){
	int i; for ( i=0 ; i < RES ; i++){
		// initialize a lookup array of values for OCR1B that form  a sin wave b/w 0 and OCR1A
		sinVals[i] = ( unsigned long )(period * ( cos( ((double)i * 2.0 * PI) / (double)( RES  ) )/2.0+.5)  ); 
	}	
}
void  setup_timer(){
	sinValsIndex=1;	
	//OCR1A=period;		// Set OCR1A to be adjusted for the frequency. 16MHz/(370hz*RES*clkdiv)
	OCR1B=0;
	clear(TCCR1B,CS12); clear(TCCR1B,CS11); set(TCCR1B, CS10);	// Setting up clock divider		
	set(TCCR1B,WGM13); set(TCCR1B,WGM12); set(TCCR1A,WGM11); set(TCCR1A,WGM10); // Set mode15 on timer 1
	set(DDRB,6); // Controll B6 with the timer
	set(TCCR1A,COM1B1); set(TCCR1A,COM1B0);	// Match b/w TCNT1 and OCR1B clears OCR1B, set at rollover
	set(TIMSK1,OCIE1A);	// call an interupt when timer 1 rolls over
	sei(); 		//enable global interupts
}
void runTest(){
	newPacket=1;
	m_green(ON);
	playSound();
}

void playSound(){
//	FREQ=400;	
//	cycles=10000;
	FREQ=(int)(*feq_ptr)/10;
	cycles=packet[2]*FREQ/100;
	OCR1A=period/FREQ ;
}

void m_rf_init(){
	m_rf_open(CHANNEL,RXADDRESS,PACKET_LENGTH);
}

ISR( TIMER1_COMPA_vect ){ next=1; }

ISR( INT2_vect)
{
	m_green(ON);
	m_rf_read(packet, PACKET_LENGTH);
	newPacket=1;
}


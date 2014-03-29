#include <avr/io.h>
#include "m_general.h"
#include "m_bus.h"
#include "m_rf.h"
#include "m_usb.h"
#include <math.h> 
#include <avr/interrupt.h>

#define 	PI		3.14159265
#define 	SYSCLK		16000000/64			// Defauld sysclk val 16MHz
#define 	CLKDIV 		0   		// required 16MHz for m_bus to work
#define 	RES    		100		// Length of array holding full sin wav.
#define 	CLKPERIOD	1000		// OCR1A
#define 	PACKET_LENGTH	3
#define 	CHANNEL		1
#define		RXADDRESS	0x26
#define 	freqRange	10000
#define 	ocr1bMAX	32

unsigned int  	FREQ;
bool		newPacket;
int 		ocr1a[freqRange];
int sinVals[100]={ 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 7, 6, 5, 4, 4, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 25, 26, 27, 28, 28, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 32, 32, 32, 32, 31, 31, 31, 30, 30, 30, 29, 28, 28, 27, 26, 25, 25, 24, 23, 22, 21, 20, 19, 18, 17};
unsigned int 	cycles ;
unsigned long 	period;
int 		sinValsIndex;
volatile int 	next;
char 		packet[3]={0,0,0};
int* 		feq_ptr;
void setVals(void );
void sinValOCR1B(int);
void setup_OCR1A();
void setup_timer(void);
void testF(int);

int main(void )
{
	m_usb_init();
	period=SYSCLK/(RES);
	sinValOCR1B(ocr1bMAX);
	m_rf_open(CHANNEL,RXADDRESS,PACKET_LENGTH);
	setup_timer();
	feq_ptr=(int*)&packet[0];
	testF(300);
	sei();

		
	while(1)
	{
		if (cycles){
			if (next==1)
			{
				OCR1B=sinVals[sinValsIndex];
				sinValsIndex++;
				if (sinValsIndex==RES){
					sinValsIndex=0;
				}


			}
		}
		if (newPacket){
			newPacket=0;
			setVals();
		}
		if(m_usb_rx_available()) // new incoming data
		{
			OCR1A=ocr1a[(int)m_usb_rx_char()];
			m_usb_tx_string("\n");
	 	}
	}
}


void sinValOCR1B(int maxB)
{
}
void  setup_timer(){
	sinValsIndex=1;	
	clear(TCCR1B,CS12); set(TCCR1B,CS11); set(TCCR1B, CS10);// Set up clock divider
	set(TCCR1B,WGM13); set(TCCR1B,WGM12); set(TCCR1A,WGM11); set(TCCR1A,WGM10); // Mode15
	set(DDRB,6);//control pinB with timer  
	set(TCCR1A,COM1B1); clear(TCCR1A,COM1B0);// TCNT1 = OCR1B clears OCR1B, set at rollover
	set(TIMSK1,OCIE1A);	// call an interupt when timer 1 rolls over
}

void setup_OCR1A(){
	int i;
	for (i=1;i<freqRange ; i++){
		ocr1a[i]=SYSCLK/((unsigned long)((i)*10));
	}
}

void setVals(){
	OCR1A=ocr1a[((int)(*feq_ptr))];
	cycles=(int)(packet[2]) * (int)(*feq_ptr) /100;
}

ISR( TIMER1_COMPA_vect )
{
	next=1; 
}

ISR( INT2_vect)
{
	m_green(ON);
	m_rf_read(packet, PACKET_LENGTH);
	newPacket=1;
}

void testF(int f)
{
	packet[0]=f%16+f%(16*16)	;
	packet[1]=f%(16*16*16)		;
}


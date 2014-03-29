#include <math.h>
#include <stdio.h>

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



int 		currFreq;
int 		sinVals[ RES ];
int 		ocr1a[ FREQ_HI-FREQ_LO+1 ];
long	 	cycles;
char 		packet[3]={ 0,0,0 };
int 		FREQ;
int OCR1A;

int OCR1B;

void setupVals(void);

int main(void){
	setupVals();
	int i;
	int j;
	for (j=300;j<=440;j++){
		printf("\n");
		OCR1A=ocr1a[j-300];
		printf("OCR1A: \t\t\t %d \n",ocr1a[j-300]);
		for (i=0; i<RES;i++){
			OCR1B=(sinVals[ i ]*OCR1A)/1000;		
			printf("freq: %d \t index: \t %d OCR1B: \t %d \n", j,i,OCR1B);
		}
	}
	return 0;
}
void setupVals(){
	int i;
	int j;
	for (j=FREQ_LO ; j<=FREQ_HI ; j++){
		ocr1a[ j-FREQ_LO ]=(int)(16000000.0/(8.0 * (float)(RES * j)));
	}
	for ( i=1 ; i <= RES ; i++){
		sinVals[i] = ( int )(( sin(((double)i * 2.0*PI)/(double)(RES))/2.0+.5) * 1000 );
	}
}	
/*	
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
}*/	

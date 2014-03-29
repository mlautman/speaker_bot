#include <stdio.h>


int main ()
{
	float 	num;
	int 	index;
	int 	c;
	int 	isNeg;
	c = 	getchar();
	while ( c != EOF ){
		c=getchar();
		if ( c == '-' ){
			isNeg=1;
		}
		else
		{
			isNeg=0;
		} 	
		c=getchar();			
		while ( c != '\n' ){
			putchar(c);
			//	num = atoi(&putchar(c));
			c=getchar();
			
		}
		index++;
	}
	return -1;	
}

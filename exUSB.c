#include "m_usb.h"
#include "m_general.h"


int main(void){
	unsigned char a=0;
	m_red(ON);
	m_clockdivide(0):
	m_usb_init();
	while(!m_usb_isconnected());
	m_red(OFF);
	

	while(1){
	if(m_usb_rx_available()){

		a=m_usb_rx_char();
		m_usb


}

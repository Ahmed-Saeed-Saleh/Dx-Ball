#include "PORTE.h"



void PORTE_init(void){
	unsigned volatile long delay ;
	SYSCTL_RCGC2_R |= (1<<4);						// port E clock
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTE_DIR_R &=~(1<<1);					//PE0 is input
	GPIO_PORTE_CR_R = 0x02;
	GPIO_PORTE_AFSEL_R &= ~(1<<1);     	// not alternative
  GPIO_PORTE_AMSEL_R &= ~(1<<1);     	// no analog
  GPIO_PORTE_PCTL_R &= ~0x0000F00F;		// bits for PD3, PD0
  GPIO_PORTE_DEN_R = (1<<1);    			// enable PD3, PD0
	GPIO_PORTE_PUR_R |= (1<<1);
}

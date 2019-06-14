
// Blue Nokia 5110
// ---------------
// Signal        (Nokia 5110) LaunchPad pin
// Reset         (RST, pin 1) connected to PA7
// SSI0Fss       (CE,  pin 2) connected to PA3
// Data/Command  (DC,  pin 3) connected to PA6
// SSI0Tx        (Din, pin 4) connected to PA5
// SSI0Clk       (Clk, pin 5) connected to PA2
// 3.3V          (Vcc, pin 6) power
// back light    (BL,  pin 7) not connected, consists of 4 white LEDs which draw ~80mA total
// Ground        (Gnd, pin 8) ground
// *************************** Header Files             ***************************

#include "Nokia5110.h"
#include "BitMaps.h"
#include "PORTF.h"
#include "PORTE.h"
#include "TExaS.h"
#include <string.h>
#include <stdlib.h>

// *************************** FUNCTIONS DEFINATIONS    ***************************

void DisableInterrupts(void); 				// Disable interrupts
void EnableInterrupts(void);  				// Enable interrupts
void Delay100ms(unsigned long count); // time delay in 0.1 seconds
void display();
void startingScreen();
void Gameover();

// *************************** Global Variables         ***************************

int right, left;
int px = 32, bx = 36, by = 36, xval = 4, yval = -4, tx, ty, temp;
int flage[21][12]; 

// *************************** Interrupt Function       ***************************

void GPIOPortF_Handler(void){ // called on touch of either SW1 or SW2
  if(GPIO_PORTF_RIS_R&0x01){  // SW2 touch
    GPIO_PORTF_ICR_R = 0x01;  // acknowledge flag0
		if(px <63){
		  Nokia5110_PrintBMP(px, 47, space, 0);
			px+=4;
		}
  }
  if(GPIO_PORTF_RIS_R&0x10){  // SW1 touch
    GPIO_PORTF_ICR_R = 0x10;  // acknowledge flag4
		if(px > 0){
			Nokia5110_PrintBMP(px + 16, 47, space, 0);
			px-=4;
		}
	}
	Nokia5110_PrintBMP(px, 47, PlayerShip0, 0);
		
}

// *************************** Main Funtion             ***************************

int main(void){
	
  TExaS_Init(SSI0_Real_Nokia5110_Scope);  // set system clock to 80 MHz
	PORTE_init();
	PORTF_init();
	display();
  while(1){
		if(by >= 43 && bx >= px && bx < px + 20){
			temp = bx - px;
			if(temp < 8){
				xval = -4;
			}else if(temp < 12 ){
				xval = 0;
			}else{
				xval = 4;
			}
			yval = -4;
		}
		Nokia5110_PrintBMP(bx, by, space, 0);
		tx = (bx + xval)/4;
		ty = (by + yval)/4;
		if(tx >= 0 && ty >= 0 && tx < 21 && ty < 12 && flage[tx][ty]){
			bx += xval;
			by += yval;
			if(xval > 0 && yval < 0){
				if(flage[tx][ty + 1] && flage[tx - 1][ty]){
					xval = -xval;
					yval = -yval;
				}else if(flage[tx - 1][ty]){
					yval = -yval;
				}else{
					xval = -xval;
				}
			}else if(xval < 0 && yval < 0){
				if(flage[tx + 1][ty] && flage[tx][ty + 1]){
					xval = -xval;
					yval = -yval;
				}else if(flage[tx + 1][ty]){
					yval = -yval;
				}else{
					xval = -xval;
				}
			}else if(xval < 0 && yval > 0){
				if(flage[tx + 1][ty] && flage[tx][ty - 1]){
					xval = -xval;
					yval = -yval;
				}else if(flage[tx + 1][ty]){
					yval = -yval;
				}else{
					xval = -xval;
				}
			}else{
				if(flage[tx - 1][ty] && flage[tx][ty - 1]){
					xval = -xval;
					yval = -yval;
				}else if(flage[tx - 1][ty]){
					yval = -yval;
				}else{
					xval = -xval;
				}
			}
			flage[tx][ty] = 0;
			Nokia5110_PrintBMP(tx * 4, ty * 4, space, 0);
		}else{
			if(bx <= 0 || bx >= 79){
				xval = -xval;
			}
			if(by <= 4){
				yval = -yval;
			}
			if(by >= 47){
				Gameover();
				return 0;
			}				
			Delay100ms(40);
			bx += xval;
			by += yval;
		}
		
		Nokia5110_PrintBMP(bx, by, ball, 0);
    Nokia5110_DisplayBuffer();
	}

}
// *************************** FUNCTIONS DECLARATION    ***************************

void display(){
	int i , j ;
	Nokia5110_Init();
	Nokia5110_Clear();
  Nokia5110_ClearBuffer();
	Nokia5110_DisplayBuffer();      							// draw buffer
	startingScreen();
	Nokia5110_Clear();
  Nokia5110_ClearBuffer();
  Nokia5110_PrintBMP(32, 47, PlayerShip0, 0); 	// player ship middle bottom
	Nokia5110_PrintBMP(bx, by, ball, 0);
  Nokia5110_DisplayBuffer();     								// draw buffer
	for(i=0;i<84;i+=4){
		for(j=8;j<24;j+=4){
			Nokia5110_PrintBMP(i, j , food, 0);
			flage[i/4][j/4] = 1;
		}
	}
	Nokia5110_DisplayBuffer();
}

void startingScreen()
{
	Nokia5110_SetCursor(0,0);
	Nokia5110_ClearBuffer();
	Nokia5110_PrintBMP(10, 35, logo, 0);
	Nokia5110_DisplayBuffer();
	Nokia5110_SetCursor(0,5);
	Nokia5110_OutString("DX BALL");	
	Delay100ms(100);
	Nokia5110_Clear();
	Nokia5110_ClearBuffer();
	Nokia5110_SetCursor(0,0);
	Nokia5110_OutString("To Play ");	
	Nokia5110_SetCursor(0,1);
	Nokia5110_OutString("Press Start");	
	Nokia5110_SetCursor(0,2);
	Nokia5110_OutString("button ");	
	while((GPIO_PORTE_DATA_R&(1<<1)));
}
void Gameover(){
	Nokia5110_ClearBuffer();
	Nokia5110_Clear();
	Nokia5110_SetCursor(2,1);
	Nokia5110_OutString("GameOver");	
	Delay100ms(10);
}

void Delay100ms(unsigned long count){unsigned long volatile time;
  while(count>0){
    time = 72724;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}

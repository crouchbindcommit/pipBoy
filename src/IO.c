#include "IO.h"
#include "tm4c123gh6pm.h"

//Place the definition for bit specific addressing here.  
#define PF1   (*((volatile uint32_t *) 0x40025008)) //aka RED
#define PF3		(*((volatile uint32_t *) 0x40025020)) //aka GREEN

//#define PF2 (*((volatile unsigned long *)0x40025010))


/*
	File to handle button input for state switching 

*/

//Should return the button states
uint32_t pushbuttons(void){
	return (GPIO_PORTF_DATA_R&0x11);
	// Set to 42 because the compiler expects a return value.  
	// Your function only needs to return a value denoting the button states
	// rather than the answer to life, the universe, and everything...
	return 42;
}





// General Purpose Delay
void delay(void){ // **Using 20MHz clock!**			// 1 cycle = 50ns 
		// 250ms =>  5,000,000 cycles		500ms => 10,000,000 cycles
	NVIC_ST_RELOAD_R = 5000000 - 1;  	// number of cycles to wait
  NVIC_ST_CURRENT_R = 0;       			// any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){} // wait for count flag
}

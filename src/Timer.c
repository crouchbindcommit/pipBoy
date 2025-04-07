//Timer.c
// Definitions for additional timer interrupts. 
// Note: Systick defined in Sound.c

#include <stdint.h>         
#include "tm4c123gh6pm.h"   
#include "Timer.h"
#include "LED.h"
#include "UART.h"
#include "Sound.h"

// Function prototypes
void Timer1A_Init(void);
void TIMER1A_Handler(void);
void EnableInterrupts(void);  // Enable interrupts

const unsigned MHLL[32] = {
	E, D, C0, D, E, E, E, N0,
	D, D, D, N0, E, E, E, N0,
	E, D, C0, D, E, E, E, N0,
	E, D, D, E, D, C0, N0, N0};

// Function to initialize Timer1A
void Timer1A_Init(void) {
  SYSCTL_RCGCTIMER_R |= 0x02;  		// Enable clock for Timer1
	while ((SYSCTL_PRTIMER_R & 0x02) == 0) {} //wait till rev instruct takes
  TIMER1_CTL_R = 0;               // Disable Timer1A during configuration
  TIMER1_CFG_R = 0;             	// Select 32-bit timer configuration
  TIMER1_TAMR_R = 0x02;           // Set TimerA to periodic mode (down-counter)
  TIMER1_TAILR_R = 60000000 - 1;	// Set TimerA interval load value
  TIMER1_ICR_R = 0x01;						// Clear the timeout flag
  TIMER1_IMR_R |= 0x01;       		// Enable TimerA timeout interrupt
	TIMER1_CTL_R |= 0x01;       		// Enable Timer1A

	NVIC_EN0_R |= (0x01 << 21);  // Enable IRQ21 (Timer1A)   
}

// Timer1A interrupt service routine
void Timer1A_Handler(void) {
  LED_GreenToggle(); 
	//Sound_Init(MHLL[beat]);
	Sound_Init(MHLL[beat]);
	beat = (beat+1)% 32;
	TIMER1_ICR_R = 0x01;
}

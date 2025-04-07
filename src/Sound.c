// Sound.c, derived from SysTickInts.c


// Use the SysTick timer to request interrupts at a particular period.

// Port B bits 2-0 have the 3-bit DAC

// Port E, SW1(PE0) and SW2(PE2)
// SysTick ISR: Calls SineWave 

#include <stdint.h>
#include "Sound.h"
#include "LED.h"
#include "UART.h"
#include "Timer.h"
#include "LED.h"
#include "tm4c123gh6pm.h"
#include "PortF.h"

unsigned char Index; 
volatile unsigned char wave = 0;
volatile unsigned char beat = 0;

//4-bit 32-element wave options
const unsigned sine[32] = {	
  8,9,11,12,13,14,14,15,15,15,14,14,13,12,
	11,9,8,7,5,4,3,2,2,1,1,1,2,2,3,4,5,7};
const unsigned piano[32] = {
	8, 10, 11, 11, 10, 9, 8, 10, 12, 13, 14, 15, 15, 15, 14,
	12, 10, 8, 6, 4, 3, 2, 1, 0, 0, 1, 1, 2, 3, 4, 5, 7};
const unsigned trumpet[32] = {
	8, 11, 13, 15, 14, 13, 12, 13, 11, 10, 10, 11, 10, 10,
	9, 9, 8, 9, 7, 6, 5, 6, 7, 7, 9, 8, 6, 3, 0, 3, 6, 8};

const unsigned *forms[3] = {sine, piano, trumpet};

//Mary had a little lamb (MHLL) song defined & used in Timer

unsigned long systick_count = 0;
unsigned long Skip_count = 0;
uint32_t paused = 1; 

// **************DAC_Init*********************
// Initialize 4-bit DAC 
// Input: none
// Output: none
void DAC_Init(void){
	unsigned long volatile delay;
  SYSCTL_RCGCGPIO_R |= 0x08; // activate port B
  delay = SYSCTL_RCGCGPIO_R; // allow time to finish activating

	GPIO_PORTD_AMSEL_R &= ~0x0F; // Disable analog on PD3-0
  GPIO_PORTD_PCTL_R &= ~0x0000FFFF; 
	GPIO_PORTD_DIR_R |= 0x0F; // make PD3-0 out         
  GPIO_PORTD_AFSEL_R &= ~0x0F; //no alt function
  GPIO_PORTD_DEN_R |= 0x0F; // enable digital I/O on PD3-0
}

// **************Sound_Init*********************
// Initialize Systick periodic interrupts, this is basically demonstrate SysTick usage
// Input: interrupt period
//        Units of period are 12.5ns
//        Maximum is 2^24-1
//        Minimum is determined by length of ISR
// Output: none
void Sound_Init(unsigned long period){
  DAC_Init();          // Port D is DAC
  Index = 0;
	// your code begins below
	NVIC_ST_CTRL_R = 0; //disable systick
	NVIC_ST_RELOAD_R = period-1; //reload value
	NVIC_ST_CURRENT_R = 0; //writing to current clears
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF) | 0x20000000; //priority 1 
  NVIC_ST_CTRL_R = 0x0007; // enable SysTick with core clock and interrupts
}

// **************DAC_Out*********************
// output to DAC
// Input: 3-bit data, 0 to 7 
// Output: none
void DAC_Out(unsigned long data){
  // write the value to Data register at PortB
	GPIO_PORTD_DATA_R = (data & 0x0F); 
}

void DAC_UnPause(void){
	paused = 0;
	 NVIC_ST_CTRL_R |= 0x00; 
	TIMER1_CTL_R |= TIMER_CTL_TAEN;
}

void DAC_PlaySong(unsigned instrument){
	if (paused == 1) return;
	uint32_t button_state = Board_Input();
	 if ((button_state & 0x11) == 0x00) {
      instrument = (instrument + 1) % 3;
    }
	wave = instrument;
	LED_Off();
	Sound_Init(10);
	Timer1A_Init();
}

void DAC_Pause(void){
	paused = 1; 
	NVIC_ST_CTRL_R = 0x00;
	TIMER1_CTL_R &= ~TIMER_CTL_TAEN;	
	Index = 0;
	beat = 0;
}

// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){
	//every iterrupt progress the waveform
  Index = (Index+1) % 0x1F; //roll over after 31
	DAC_Out(forms[wave][Index]); 
	systick_count++;
}

unsigned GetInstrument(void){
	return wave;
}



// Sound.h, 

// Runs on LM4F120 or TM4C123, 
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano
// September 15, 2013
// Port B bits 2-0 have the 3-bit DAC
// Port F is onboard LaunchPad switches and LED
// Port F bit 4 is negative logic switch to play sound, SW1
// Piano: PF3 ISR heartbeat

extern volatile unsigned char beat;
extern volatile unsigned char wave;

// **************DAC_Init*********************
// Initialize 3-bit DAC 
// Input: none
// Output: none
void DAC_Init(void);

// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Input: interrupt period
//        Units of period are 12.5ns
//        Maximum is 2^24-1
//        Minimum is determined by lenght of ISR
// Output: none
void Sound_Init(unsigned long period);

// bus is 80 MHz
// Period =  80000000/(31 * NoteFreq)
#define C   	2466   // 1046.5 Hz		
#define B   	2613   // 987.8 Hz		
#define BF  	2768   // 932.3 Hz		
#define A   	2933   // 880 Hz		
#define AF  	3107   // 830.6 Hz		
#define G   	3292   // 784 Hz		
#define GF  	3487   // 740 Hz		
#define F   	3695   // 698.5 Hz		
#define E   	3914   // 659.3 Hz		
#define EF  	4147   // 622.3 Hz		
#define D   	4394   // 587.3 Hz		
#define DF  	4655   // 554.4 Hz		
#define C0  	4932   // 523.3 Hz		
#define N0  	  15	 // > 50000 Hz (52) 



// **************DAC_Out*********************
// output to DAC
// Input: 3-bit data, 0 to 7 
// Output: none
void DAC_Out(unsigned long data);

// ***********DAC_PlaySong*******************
// begins song playing by enabling systick interrupts 
// input: instrument selection
// output: none
void DAC_PlaySong(unsigned instrument);
void DAC_Pause(void);
void DAC_UnPause(void);
unsigned GetInstrument(void);

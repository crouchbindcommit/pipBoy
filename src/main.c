/* Anika Roskowski 
 * Mak Mckinney
 * 455 Final Project
 * Created: 11/18/2024
 * Modified: 
 * 
 * Working PIP-Boy Model
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "pll.h"
#include "UART.h"
#include "ST7735.h"
#include "esp8266.h"
#include "LED.h"
#include "weather.h"
#include "Sound.h"
#include "RTC.h"
#include "Timer.h"
#include "PortF.h"

// prototypes for functions defined in startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

//debug selections
bool show_Ucmd = false; 	//show ESP commands to UART0

// Global variables to track current and previous pages
uint32_t currentPage = 0;
uint32_t previousPage = -1; // Initialized to an out-of-range value
volatile uint8_t left_button_pressed;
volatile uint8_t right_button_pressed;

	
void drawStill() {
  uint16_t blue = ST7735_BLUE;   // Background color (dark blue)
  uint16_t yellow = ST7735_YELLOW; // Vault-Tec yellow

  // Define the gear + "VT" pixel art in a 5x7 grid
  uint8_t gearDesign[7][5] = {
    {0, 1, 1, 1, 0}, // Top teeth
    {1, 0, 0, 0, 1}, // Sides
    {1, 1, 0, 1, 1}, // "V"
    {1, 0, 1, 1, 1}, // "T"
    {1, 0, 0, 0, 1}, // Sides
    {0, 1, 1, 1, 0}, // Bottom teeth
    {0, 0, 0, 0, 0}  // Optional empty row
  };

  // Scale factor (each grid cell is 16x16 pixels)
  int scale = 16;
  int designWidth = 5 * scale;
  int designHeight = 7 * scale;

  // Calculate offsets to center the design
  int offsetX = (128 - designWidth) / 2;
  int offsetY = (160 - designHeight) / 2;

  // Clear screen with blue background in one operation
  ST7735_FillScreen(blue);

  // Draw the design on the screen
  for (int row = 0; row < 7; row++) {
    for (int col = 0; col < 5; col++) {
      uint16_t color = gearDesign[row][col] ? yellow : blue;

      // Map each grid cell to a scaled block on the screen
      for (int py = 0; py < scale; py++) {
        for (int px = 0; px < scale; px++) {
          ST7735_DrawPixel(
            offsetX + col * scale + px,
            offsetY + row * scale + py,
            color
          );
        }
      }
    }
  }
}
void drawNote() {
  uint16_t black = ST7735_YELLOW;  // Background color (black)
  uint16_t white = ST7735_BLUE;  // Music note color (white)

  // Define the music note pixel art in a 5x7 grid
  uint8_t musicNote[7][5] = {
    {0, 1, 1, 1, 0}, // Top of the note (rounded part)
    {0, 1, 1, 1, 0}, // Top of the note (rounded part)
    {0, 1, 0, 1, 0}, // Stem start
    {0, 1, 0, 1, 0}, // Stem continuation
    {0, 1, 0, 1, 0}, // Stem continuation
    {0, 1, 1, 1, 0}, // Stem base
    {0, 0, 0, 0, 0}  // Optional empty row
  };

  // Scale factor (each grid cell is 16x16 pixels)
  int scale = 16;
  int designWidth = 5 * scale;
  int designHeight = 7 * scale;

  // Calculate offsets to center the design
  int offsetX = (128 - designWidth) / 2;
  int offsetY = (160 - designHeight) / 2;

  // Only draw the design on the screen, without clearing the screen first
  for (int row = 0; row < 7; row++) {
    for (int col = 0; col < 5; col++) {
      uint16_t color = musicNote[row][col] ? white : black;

      // Map each grid cell to a scaled block on the screen
      for (int py = 0; py < scale; py++) {
        for (int px = 0; px < scale; px++) {
          ST7735_DrawPixel(
            offsetX + col * scale + px,
            offsetY + row * scale + py,
            color
          );
        }
      }
    }
  }
}


void showState(uint32_t state, unsigned instrument) {
  // Only redraw if the state has changed
  if (state != previousPage) {
    previousPage = state;  // Update previousPage
			uint16_t blue = ST7735_BLUE;  
			ST7735_FillScreen(blue);
	
		
    if (state == 0) {
      DAC_Pause();
      drawStill();  // Draw background for state 0
    } else if (state == 1) {
      DAC_Pause();
      printf("Getting weather...\n");
      GetWeather();  // Start fetching weather asynchronously
      PrintHeader();
      PrintWeather();
    } else if (state == 2) {
      DAC_Pause();
			
      printf("Getting time...\n");
      DS1307_SetDateTime(0x11, 0x00, 0x55);  // Set date/time (optional)
      DS1307_PrintDateTime();  // Print time
		
    } else if (state == 3) {
      DAC_UnPause();
      drawNote();
      DAC_PlaySong(instrument);  // Play the song
    }
  }
}




int main(void) {
  // Initialization code
  DisableInterrupts();
  PLL_Init(Bus80MHz);
  LED_Init(); // Init port F and turn on RedLED (Busy)
  UART_Init(); // UART0 for debugging ESP - will always show working status
  LED_BlueOn();
  ST7735_InitR(INITR_REDTAB); // Init screen (auto set black)
  ESP8266_Init(115200, show_Ucmd); // Connect to access point, set up as client
  ESP8266_GetVersionNumber();
  I2C2_init();
	DAC_Init();  // Initialize DAC if needed
  Timer1A_Init();  // Timer setup
	left_button_pressed = 0;
	right_button_pressed = 0;
  EnableInterrupts();


  unsigned instrument = 0;

  while (1) {
    uint32_t button_state = Board_Input();
    // Increment the timer (use a hardware timer if possible)
    //delay = SYSCTL_RCGCGPIO_R;

    /* No Buttons Pressed */
    if ((button_state & 0x11) == 0x11) {
      left_button_pressed = 0;  // Reset the button press state
      right_button_pressed = 0; // Reset the button press state
    }

    /* Left Button Pressed */
    else if ((button_state & 0x01) == 0x01) {  // Left button press
      if (!left_button_pressed) {
        if (currentPage < 3) currentPage++;
        else currentPage = 0;
        left_button_pressed = 1;
				UART_OutString("Left button pressed, current page:");
				UART_OutUDec(currentPage);
      }
    }

    /* Right Button Pressed */
    else if ((button_state & 0x10) == 0x10) {  // Right button press
      if (!right_button_pressed) {
        if (currentPage > 0) currentPage--;
        else currentPage = 3;
        right_button_pressed = 1;
      }
    }

    /* Both Buttons Pressed */
    else {
     // currentPage = 4;  // Set screen to white when both buttons are pressed
      left_button_pressed = 1;
      right_button_pressed = 1;
//if (instrument < 2) instrument++;
  //    else instrument = 0;
    }

    // Reset button press states when buttons are released
    if ((button_state & 0x01) == 0) left_button_pressed = 0;
    if ((button_state & 0x10) == 0) right_button_pressed = 0;

    // Update the screen based on the current state, with non-blocking delays
    showState(currentPage, instrument);
  }
}

	
/* Wiring Diagram: 

ESP8266 Wifi Module:

Tiva 	| 	ESP		| 	Description
------------------------------------------
3.3V	| 	3V3		| Power Supply
PB1		| 	RX		| UART from Tiva @ 115200BR
PB5		| 	RST		| Low = HW Reset
N/A 	| 	IO0 	| +3.3V norm (GND flash) 
N/A 	| 	EN		| BreadBoard conn: 10K to 3.3V
N/A 	| 	IO2 	| +3.3V
PB0 	| 	TX		| UART to Tiva @ 115200BR
GND 	| 	GND		| its just ground :) 


ST7735R LCD Display:

Tiva 	| ST7735R	| 	Description
------------------------------------------
3.3V	|  Lite		| Power Supply
N/A		|  MISO		| -----
PA2		|  SCK| 	| SSIOClk
PA5		|  MOSI		| SSIOTx
PA3		| TFT_CS	| SSIOCFss
N/A		| CARD_CS | -----
PA6		|  D/C		| Data (H) / Command (L) 
PA7		|  RESET	| Reset LCD
3.3V	|  VCC		| Power Supply
GND 	|  GND		| its just ground :) 

*/

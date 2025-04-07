// RTC.c 
// Driver for the external DS1307 RTC module 
// Uses I2C communication protocol (I2C2 PE4&5)

#include <stdio.h>
#include <stdint.h>
#include "RTC.h"
#include "tm4c123gh6pm.h"
#include "ST7735.h"


char readTime[3]; 

//------------I2C2_Init------------
// Initialize I2C Module 2 on PE4&5 (assuming 80 MHz clock),
// Input: none
// Output: none
void I2C2_init(void){
	SYSCTL_RCGCGPIO_R |= 0x10;			//activate Port E
	SYSCTL_RCGCI2C_R |= 0x04; 			//Enable I2C2 Clock
  while((SYSCTL_PRGPIO_R & 0x10)!=0x10){}; // wait to finish activating

  GPIO_PORTE_AFSEL_R |= 0x30;			// disable alt funct on PF0 and PF4
  GPIO_PORTE_ODR_R |= 0x20;				// Open drain on PE5 (SDA) SCL will show same behavior
	GPIO_PORTE_DEN_R |= 0x30;				// enable digital I/O on PE4&5
	GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R & ~0x00FF0000) | 0x00330000;
	//Configure pins as I2C 
	
	I2C2_MCR_R = 0x10; 							// Master Enabled
	I2C2_MTPR_R = 39; 							// Standard Mode 100kHz @ 80MHz bus
}

//wait until master has completed
static int I2C_waitComplete(void){
	while(I2C2_MCS_R & 0x01){};
	return(I2C2_MCS_R & 0xE);
}

//------------I2C2_burstWrite------------
// write multiple bytes to consecuative locations
// Input: slave addr, starting memaddr, #bytes sending, data to send
// Output: error code or 0 
char I2C2_burstWrite(int slaveAddr, char memAddr, int byteCount, char *data){
	char error;
	
	if(byteCount <= 0) return -1; //no write
	
	//give master slave and starting addresses
	I2C2_MSA_R = slaveAddr << 1;
	I2C2_MDR_R = memAddr;
	I2C2_MCS_R = 3; //Initiate transmition: set START and RUN
	
	//wait until completion, check for error
	error = I2C_waitComplete();
	if(error) return error;
	
	//send data byte by byte
	while(byteCount > 1){
		I2C2_MDR_R = *data++;			//store memory address 
		I2C2_MCS_R = 1; 					//initiate transmition (RUN)
		error = I2C_waitComplete(); //check for errors
		if(error) return error;
		byteCount--;
	}
	
	//send last byte and stop
	I2C2_MDR_R = *data++;				//store memory address 
	I2C2_MCS_R = 5; 							//initiate transmition (STOP & RUN)
	error = I2C_waitComplete(); //check for errors
	
	while(I2C2_MCS_R & 0x40){}; //let bus complete
	
	//return
	if(error) return error;
	return 0;
}
	
char I2C2_read(int slaveAddr, char memAddr, int byteCount, char *data){
	char error;
	
	if (byteCount <= 0) return -1; //could not read
	
	//give master slave and starting addresses
	I2C2_MSA_R = slaveAddr << 1;
	I2C2_MDR_R = memAddr;
	I2C2_MCS_R = 3; //Initiate transmition: set START and RUN
	
	//wait until completion, check for error
	error = I2C_waitComplete();
	if(error) return error;
	
	//set to read mode - restart with slaveAddr
	I2C2_MSA_R = (slaveAddr << 1) + 1;
	
	//dont acknowledge last byte, else do 
	if(byteCount == 1) I2C2_MCS_R = 0x07;
	else	I2C2_MCS_R = 0x0B; 
	
	//wait until completion, check for error
	error = I2C_waitComplete();
	if(error) return error;
	
	*data++ = I2C2_MDR_R;	 	//store recieved data
	
	if(--byteCount == 0){		//if last byte, we are done
		while(I2C2_MCS_R & 0x40); //wait till bus complete
		return 0;									//exit with no error
	}
	
	while(byteCount > 1){
		I2C2_MCS_R = 0x09; 	//data ack
		//wait until completion, check for error
		error = I2C_waitComplete();
		if(error) return error;
		byteCount--;
		*data++ = I2C2_MDR_R;
	}
	
	I2C2_MCS_R = 5;
	error = I2C_waitComplete(); //check for errors
	
	*data = I2C2_MDR_R;
	while(I2C2_MCS_R & 0x40){}; //let bus complete
	
	//return
	if(error) return error;
	return 0;
}
	
void drawHourglass() {
  uint16_t black = ST7735_YELLOW;  // Background color (yellow)
  uint16_t white = ST7735_BLUE;    // Hourglass color (blue)

  // Define the hourglass pixel art in a 7x7 grid
  uint8_t hourglass[7][7] = {
    {0, 0, 1, 1, 1, 0, 0}, // Top part (wide)
    {0, 1, 1, 1, 1, 1, 0}, // Second row
    {1, 1, 1, 1, 1, 1, 1}, // Middle part (full width)
    {0, 1, 1, 1, 1, 1, 0}, // Bottom part
    {0, 0, 1, 1, 1, 0, 0}, // Narrow bottom part
    {0, 0, 0, 0, 0, 0, 0}, // Optional empty row
    {0, 0, 0, 0, 0, 0, 0}  // Optional empty row
  };

  // Scale factor (each grid cell is 16x16 pixels)
  int scale = 16;
  int designWidth = 7 * scale;
  int designHeight = 7 * scale;

  // Calculate offsets to center the design
  int offsetX = (128 - designWidth) / 2;
  int offsetY = (160 - designHeight) / 2;

  // Only draw the design on the screen, without clearing the screen first
  for (int row = 0; row < 7; row++) {
    for (int col = 0; col < 7; col++) {
      uint16_t color = hourglass[row][col] ? white : black;

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

void DS1307_SetDateTime(char hour, char min, char sec){
	char timeToSet[3] = {sec, min, hour};
	/* use burst write to write seconds, minutes, and hours */
  I2C2_burstWrite(SLAVE_ADDR, 0, 3, &timeToSet[0]);
}

void DS1307_PrintDateTime(void) {
	drawHourglass();
  char timeBuffer[20];  // Buffer to hold formatted time
	
  // Read time from DS1307
  I2C2_read(SLAVE_ADDR, 0, 3, readTime);  // Assuming the I2C read function works
  // Format the time as HH:MM:SS
  sprintf(timeBuffer, "Time: %02x:%02x:%02x", readTime[2], readTime[1], readTime[0]);
  
  // Set cursor position and print time on the screen
  //ST7735_SetCursor(0, 4);
  ST7735_DrawString(4,6, timeBuffer, ST7735_YELLOW);  // Print formatted time to screen
}



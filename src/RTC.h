//RTC.H
//Driver for the RTC operated through I2C 
#ifndef RTC_H
#define RTC_H

#define SLAVE_ADDR 		0b01101000 		//From DS1307 DataSheet

void I2C2_init(void); 
char I2C2_burstWrite(int slaveAddr, char memAddr, int byteCount, char *data); 
char I2C2_read(int slaveAddr, char memAddr, int byteCount, char *data);
void DS1307_SetDateTime(char hour, char min, char sec);
void DS1307_PrintDateTime(void);

#endif

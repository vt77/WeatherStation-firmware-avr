
#include <stdint.h>
extern "C" {
#include "utility/twi.h" //Arduino Wire lib
}

#define AS5600_ADRESS 0x36
#define RAW_POSITION_REG 0x0C
#define I2C_COUNTER_ADDRESS 0x15

uint16_t read_counter()
{
	static uint16_t last_counter_value = 0;
	uint16_t counter;
	
	size_t size = twi_readFrom(I2C_COUNTER_ADDRESS, (uint8_t*)&counter, 2, true);
	if(size != 2)
	{
		logger::error("[COUNTER]Counter read return %d",size);
		return 0;
	}
	
	//Wire.requestFrom(I2C_COUNTER_ADDRESS, 2);
	//counter = Wire.read();
	//counter <<= 8;
	//counter += Wire.read();

    logger::debug("[COUNTER] %x",counter);

	if(last_counter_value == 0 || counter < last_counter_value)
	{
		last_counter_value = counter;
		return 0;	
	}

	uint16_t diff  = counter - last_counter_value;
	last_counter_value = counter;
	return diff;
}

uint8_t read_wind_direction(){
	uint8_t buffer;
	buffer = RAW_POSITION_REG;
	uint8_t res = twi_writeTo(AS5600_ADRESS, &buffer, 1, true, true);
	if(res !=0)
	{
	   logger::error("[AS5600]Write AS5600 error : %d",res);
	   return 0xFF;	   
	}
	delay(10);
	size_t size = twi_readFrom(AS5600_ADRESS, &buffer, 1, true);
	if(size != 1)
	{
		logger::error("[AS5600]Wrong size readed : %d",size);
		return 0xFF;
	}

	return ((buffer+NORTH_OFFSET) & 0x0F);
}

uint16_t read_wind_speed(){
	//Assumes it called once 4 seconds and 1 revolution per m/sec
	return read_counter() / 4;
}

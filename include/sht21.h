
#include <stdint.h>

#define I2C_SHT21_ADDRESS 0x40
#define COMMAND_TEMPERATURE 0XF3
#define COMMAND_HUMIDITY  0xF5
#define SHT21_CRC8_POLYNOMINAL 0x13100;  // P(x)=x^8+x^5+x^4+1

uint8_t calc_crc(uint16_t data)
{
  for (uint8_t bit = 0; bit < 16; bit++)
  {
    if(data & 0x8000) 
    {
	    data = (data << 1) ^ SHT21_CRC8_POLYNOMINAL;
    }else{ 
	    data <<= 1;
    }
  }
  return data >>= 8;
}

void setHeater(int onoff)
{
	uint8_t buffer[3];
	buffer[0] = 0xE6; //Write settings 
	buffer[1] = onoff == 1 ? 0x04 : 0x0; //HeaterBit
	uint8_t res = twi_writeTo(I2C_SHT21_ADDRESS, buffer, 2, true, true);
}


int16_t get_temperature(){
	uint8_t buffer[3];
	buffer[0] = COMMAND_TEMPERATURE;
	uint8_t res = twi_writeTo(I2C_SHT21_ADDRESS, buffer, 1, true, true);
	delay(100);
	size_t size = twi_readFrom(I2C_SHT21_ADDRESS, buffer, 3, true);
	//Serial.println(buffer[0],HEX);
	//Serial.println(buffer[1],HEX);
	uint16_t t =  (buffer[0] << 8) + buffer[1];
	uint8_t crc = calc_crc(t);
	if(crc != buffer[2]){
		logger::error("[SHT21][TEMP] CRC Error 0x%X got : 0x%X",crc,buffer[2]);
		return VALUE_ERROR;
	}
	t &= ~0x0003;
        return (uint16_t)((-46.85 + 175.72/65536 * (float)t) * 10);	
}

uint16_t  get_humidity(){
	uint8_t buffer[3];
	buffer[0] = COMMAND_HUMIDITY;
	uint8_t res = twi_writeTo(I2C_SHT21_ADDRESS, buffer, 1, true, true);
	delay(50);
	size_t size = twi_readFrom(I2C_SHT21_ADDRESS, buffer, 3, true);
	uint16_t rh =  (buffer[0] << 8) + buffer[1];
	uint8_t crc = calc_crc(rh);
	if(crc != buffer[2]){
                logger::error("[SHT21][HUM] CRC Error 0x%X got : 0x%X",crc,buffer[2]);
		return VALUE_ERROR;
        }

  	rh &= ~0x0003;
  	return ( 0.001907 * (float)rh - 6);
}

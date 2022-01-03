//Static preferences (defaults)
#include "preferences.h"

#include <Arduino.h>
#include <RFTransmitter.hpp>
#include "tasker.hpp"
#include <Wire.h>

#include "logger.h"
#include "sht21.h"
#include "windproc.h"


using namespace vt77;

RFTransmitter rfTransmitter({500,8,4,2});
Tasker tasker;


#define DATA_LENGTH 7
#define PROTO_NUMBER 1


class WeatherData
{
    public:
         WeatherData(){
         };

        operator RFTransmitter::send_buffer_t(){
            
            buffer[0] = device_id >> 8; 
            buffer[1] = ( device_id & 0xF0 ) | PROTO_NUMBER;
            buffer[2] = (battery << 4 )| ( wind_dir & 0x0F ); 
            buffer[3] = wind_speed; 
            buffer[4] = temperature;
            buffer[5] = humidity;
            buffer[6] = 0xa5;
            for(uint8_t i=0;i<DATA_LENGTH-1;i++)
                buffer[6] ^= buffer[i];
            return buffer;
        }
        const uint8_t length = DATA_LENGTH;
        uint8_t wind_dir;    //0-F only 4 bit used
        uint8_t wind_speed;  //Format: 7 bits 0-32 with last two decimal;
        uint16_t temperature; //Format: 6 bits -30 <=> +38 with two bit decimal 
        uint16_t humidity;    //Format: 7 bits bais with one bit decimal
        uint8_t pressure;    //Format: Pressure - 650
        uint8_t battery;     //Voltage - 4 bits  2 bits decimal

 
    private:
        char buffer[DATA_LENGTH];
        const uint16_t device_id = DEVICE_ID;

};

WeatherData weatherdata;

#define ADC_REFERENCE_VALUE 1126400L


long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = ADC_REFERENCE_VALUE / result; // Back-calculate AVcc in mV
  return result;
}
 
void setup() {

    Serial.begin(115200);
    pinMode(RF_PIN, OUTPUT);
    Wire.begin();
	//PIC12 Counter too slow to work on 100kHz	
	TWBR = 150;    
    tasker.attach(RFSEND_INTERVAL ,[](){

        //Read values 
        weatherdata.battery = (readVcc() - 2000L ) / 250; 
        weatherdata.temperature = get_temperature();
        weatherdata.humidity = get_humidity();
        weatherdata.wind_dir = read_wind_direction();
        weatherdata.wind_speed = read_wind_speed();
        //Transmit data 
        rfTransmitter.send(weatherdata,weatherdata.length,RFSEND_REPEATS);
    });
}

void loop() {
    tasker.process();
}

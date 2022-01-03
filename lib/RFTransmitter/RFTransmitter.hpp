#include "stdint.h"
#include <util/delay.h>

namespace vt77
{

typedef struct{
   uint16_t osc;  /* Oscillator */
   uint8_t  gap;
   uint8_t  one;
   uint8_t  zero;
}rftimings_t;


//#define TRANSMITTER_ON()  PORTB |= (1<<RF_PIN)
//#define TRANSMITTER_OFF() PORTB &= ~(1<<RF_PIN)

#define TRANSMITTER_ON()    digitalWrite(RF_PIN,HIGH) 
#define TRANSMITTER_OFF()   digitalWrite(RF_PIN,LOW)
#define DELAY(a)            delayMicroseconds(a)


#define START_PERIOD 2

//NOTE : 
// avr _delay function works better with predefined constatnts
// but this will ignore timings.osc parameter 
#ifndef TIMINGS_OSC
#warning("TIMINGS_OSC not defined using default 500")
#define TIMINGS_OSC 100
#endif
//You can use this otherwise
//#define TIMINGS_OSC  timings.osc


    class RFTransmitter{


        public:
            typedef const char* send_buffer_t;
            RFTransmitter(const rftimings_t &t) : timings(t)
            {
                
            }

            /**
             *  Sends data over RF transmitter
             *
             *  @param data array of bytes to send
             *  @param len data length in bytes 
             *  @param repeat number of packet send repeats
             */
            void send(send_buffer_t data, const uint8_t len,int8_t repeat)
            {

                //Preambule
                //send_pulse();
                //_delay_us(START_PERIOD);
                do {
                    for(uint8_t ptr=0;ptr<len;ptr++)
                        send_byte(data[ptr]);

                    send_pulse();
                    //Gap end of packet
                    for(uint8_t i=0;i<8/*timings.gap*/;i++)
                                    _delay_us(TIMINGS_OSC);
                }while( --repeat >0 );
            }

        private:
            const rftimings_t &timings;

            void send_pulse()
            {
                TRANSMITTER_ON();
                DELAY(TIMINGS_OSC);
                TRANSMITTER_OFF();
            }


            void send_byte(uint8_t byte)
            {
                for(uint8_t i=0;i<8;i++)
                {
                    uint8_t periods  = byte & 0x80 ? 4:2;//timings.one : timings.zero;
                    send_pulse();
                    //Gap according to bit value
                    do{
                        DELAY(TIMINGS_OSC);
                    }while(--periods);           
                    byte = byte << 1;
                }
            };
        };
}
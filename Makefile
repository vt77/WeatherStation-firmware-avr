

all:
	pio run

upload:
	pio run --target upload --upload-port /dev/ttyUSB0

monitor:
	minicom -b 115200 -o -D /dev/ttyUSB0

utest:
	pio test -c ci.ini
 
exdecode:
	pio device monitor -b115200 

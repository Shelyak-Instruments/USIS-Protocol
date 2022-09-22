#ifndef __USIS_DESKTOP_BAREMETAL
#define __USIS_DESKTOP_BAREMETAL

#include <stdio.h>
#include <stdint.h>

long millis( );
long micros( );

void init( );
void loop( );

// :: SERIAL ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

class Stream {
public:
	virtual void write( uint8_t b ) = 0;
	virtual int read( ) = 0;
};

class SerialStream : public Stream {
	FILE* in;
	FILE* out;
	
public:
	explicit SerialStream( );
	void begin( int );

	virtual void write(uint8_t ch) override;
	virtual int read() override;
};

extern SerialStream Serial;

// :: PINS ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

enum PinMode {
	INPUT           = 0x0,
	OUTPUT          = 0x1,
	INPUT_PULLUP    = 0x2,
	INPUT_PULLDOWN  = 0x3,
};

enum PinStatus {
	LOW     = 0,
	HIGH    = 1,
	CHANGE  = 2,
	FALLING = 3,
	RISING  = 4,
};

void pinMode( unsigned ulPin, PinMode ulMode);
void digitalWrite( unsigned ulPin, PinStatus ulVal);

#define PIN_LED	0

#endif //__USIS_DESKTOP_BAREMETAL
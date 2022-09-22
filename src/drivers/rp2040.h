/**
* @file rp2040.h
* arduino functions emulation
*/

#ifndef __USIS_RP2040_BAREMETAL_H
#define __USIS_RP2040_BAREMETAL_H

#include <stdint.h>
#include <stdlib.h>

#include <hardware/gpio.h>
#include <hardware/uart.h>
#include <hardware/sync.h>
#include <hardware/timer.h>
#include <hardware/watchdog.h>
#include <hardware/adc.h>
#include <hardware/i2c.h>
#include <hardware/exception.h>

#include <pico/time.h>

//#define count_of( x ) ( sizeof( x ) / sizeof( ( x )[0] ) )

#define PIN_LED ( 25u )

typedef __uint32_t uint32_t;
typedef __uint8_t uint8_t;

typedef unsigned uint;

uint64_t micros();
uint32_t millis();

void init();
void loop();

// :: SERIAL ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

class Stream {
public:
	virtual void write( uint8_t b ) = 0;
	virtual int read() = 0;
};

class HardwareSerial : public Stream {
public:
	HardwareSerial();

	void begin( int speed );
	void write( uint8_t b ) override;
	int read() override;
};

extern HardwareSerial Serial;

// :: PINS ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

enum PinMode
{
	INPUT = 0x0,
	OUTPUT = 0x1,
	INPUT_PULLUP = 0x2,
	INPUT_PULLDOWN = 0x3,
};

enum PinStatus
{
	LOW = 0,
	HIGH = 1,
	CHANGE = 2,
	FALLING = 3,
	RISING = 4,
};

void pinMode( unsigned ulPin, PinMode ulMode );
void digitalWrite( unsigned ulPin, PinStatus ulVal );
PinStatus digitalRead( unsigned ulPin );

#endif //__USIS_RP2040_BAREMETAL_H
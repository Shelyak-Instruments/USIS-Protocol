/**
* @file desktop.cpp
* @author Etienne Cochard 
* Quick and dirty emulation
**/

#include <ctime>
#include <stdint.h>
#include <sys/time.h>

#include "desktop.h"

/**
 * 
 */

long millis( ) {
	return clock( );
}

/**
 * 
 */

long micros( ) {

	static uint64_t start = 0;

	struct timeval now;
    gettimeofday(&now, NULL);
    uint64_t timestamp = ((uint64_t)now.tv_sec * 1000000 + (uint64_t)now.tv_usec);

	if( start==0 ) {
		start = timestamp;
		return 0;
	}

	return timestamp-start;
}

/**
 * 
 */

int main( ) {
	init( );

	while( true ) {
		loop( );
	}

	return 0;
}

/**
 * 
 */

SerialStream::SerialStream( ) {
	in = stdin;
	//in = fopen( "build-desktop/test.txt", "r" );
	out = stdout;
}

/**
 * 
 */

void SerialStream::begin( int ) {
}

/**
 * 
 */

void SerialStream::write(uint8_t ch)  {
	putc( ch, out );
}

/**
 * 
 */

int SerialStream::read() {
	return getc( in ); 
}

/**
 * 
 */

void pinMode( unsigned ulPin, PinMode ulMode) {
}

/**
 * 
 */

void digitalWrite( unsigned ulPin, PinStatus ulVal) {
	printf( "PIN(%d) = %d\n", ulPin, ulVal );
}

/**
 * well
 */

PinStatus digitalRead( unsigned ulPin ) {
	return LOW;
}
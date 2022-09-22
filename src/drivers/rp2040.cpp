
#include "rp2040.h"
#include <hardware/timer.h>
#include <tusb.h>

/**
 * Entry point,
 * simulate init & loop
 */

int main() {
	init();

	while( true ) {
		loop();
		tud_task();
	}

	return 0;
}

/**
 * @return time from start in ms
 */

uint32_t millis() {
	return to_ms_since_boot( get_absolute_time() );
}

/**
 * @return time from start in µs
 */

uint64_t micros() {
	return to_us_since_boot( get_absolute_time() );
}

// :: Serial implementation ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

HardwareSerial::HardwareSerial() {
}

void HardwareSerial::begin( int speed ) {
	stdio_usb_init();
}

int HardwareSerial::read() {
	return getchar_timeout_us( 1 );
}

void HardwareSerial::write( uint8_t ch ) {
	putchar_raw( ch );
}

HardwareSerial Serial;

// :: Pins implementation ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//		taken from arduino library
 
static PinMode _pm[30];

void pinMode( unsigned ulPin, PinMode ulMode ) {
	switch( ulMode ) {
		case INPUT:
			gpio_init( ulPin );
			gpio_set_dir( ulPin, false );
			gpio_disable_pulls( ulPin );
			break;
		case INPUT_PULLUP:
			gpio_init( ulPin );
			gpio_set_dir( ulPin, false );
			gpio_pull_up( ulPin );
			gpio_put( ulPin, 0 );
			break;
		case INPUT_PULLDOWN:
			gpio_init( ulPin );
			gpio_set_dir( ulPin, false );
			gpio_pull_down( ulPin );
			gpio_put( ulPin, 1 );
			break;
		case OUTPUT:

			gpio_init( ulPin );
			gpio_set_dir( ulPin, true );
			break;
		default:
			return;
	}

	if( ulPin > 29 ) {
		return;
	}

	_pm[ulPin] = ulMode;
}

void digitalWrite( unsigned ulPin, PinStatus ulVal ) {
	if( ulPin > 29 ) {
		return;
	}
	gpio_set_function( ulPin, GPIO_FUNC_SIO );

	if( _pm[ulPin] == INPUT_PULLDOWN ) {
		if( ulVal == LOW ) {
			gpio_set_dir( ulPin, false );
		}
		else {
			gpio_set_dir( ulPin, true );
		}
	}
	else if( _pm[ulPin] == INPUT_PULLUP ) {
		if( ulVal == HIGH ) {
			gpio_set_dir( ulPin, false );
		}
		else {
			gpio_set_dir( ulPin, true );
		}
	}
	else {
		gpio_put( ulPin, ulVal == LOW ? 0 : 1 );
	}
}

PinStatus digitalRead( unsigned ulPin ) {
	if( ulPin > 29 ) {
		return LOW;
	}

	return gpio_get( ulPin ) ? HIGH : LOW;
}


#include "tools.h"

#include <math.h>

/**
 * check if 2 strings are the same
 * allow NULL as parameter
 * @return true if equals
 */

bool str_eq( const char* s1, const char* s2 ) {

	if( !s1 || !s2 ) {
		return s1==s2;
	}

	while( *s1 && *s1 == *s2 ) {
		s1++;
		s2++;
	}

	return *s1 == 0;
}

/**
 * basic float to string conversion
 * the buffer must be big enough to contains the number
 * return last used character
 * ie:  "1200.55\0"
 *              ^ here
 * handles Nan & Inf values
 */

char* f_to_str( float number, int digits, char* buffer ) {
	if( digits < 0 )
		digits = 2;

	if( isnan( number ) ) {
		buffer[0] = 'N';
		buffer[1] = 'a';
		buffer[2] = 'N';
		buffer[3] = 0;
		return buffer+4;
	}

	if( isinf( number ) ) {
		buffer[0] = 'I';
		buffer[1] = 'n';
		buffer[2] = 'f';
		buffer[3] = 0;
		return buffer+4;
	}
		
	if( number < -4294967040.0 || number > 4294967040.0 ) {
		buffer[0] = 'I';
		buffer[1] = 'n';
		buffer[2] = 'f';
		buffer[3] = 0;
		return buffer+4;
	}

	// Handle negative numbers
	if( number < 0.0 ) {
		*buffer++ = '-';
		number = -number;
	}

	// Round correctly so that print(1.999, 2) prints as "2.00"
	double rounding = 0.5;
	for( int i = 0; i < digits; ++i )
		rounding /= 10.0;

	number += rounding;

	// Extract the integer part of the number and print it
	unsigned long int_part = (unsigned long)number;
	double remainder = number - (double)int_part;
	buffer = i_to_str( int_part, buffer );
	
	// Print the decimal point, but only if there are digits beyond
	if( digits > 0 ) {
		*buffer++ = '.';
	}

	// Extract digits from the remainder one at a time
	while( digits-- > 0 ) {
		remainder *= 10.0;
		unsigned int toPrint = (unsigned int)remainder;
		*buffer++= toPrint+'0';
		remainder -= toPrint;
	}

	*buffer = 0;
	return buffer;
}

/**
 * basic int to string conversion
 * the buffer must be big enough to contains the number
 * return last used character
 * ie:  "1200\0"
 *           ^ here
 * 
 */

char* i_to_str( int n, char* dest ) {
	
	if( n==0 ) {
		*dest++ = '0';
		*dest = 0;
		return dest;
	}

	char temp[32+1+1];	// +1 for 0 zero term, +1 for neg if any
	char* p = temp;
	
	if( n<0 ) {
		*dest++ = '-';
	}
	
	while( n>0 ) {
		int v = n%10;
		*p++ = v+'0';
		n = n/10;
	}

	if( p>temp ) {	// !necessary
		do {
			p--;
			*dest++ = *p;
		} while( p!=temp );
	}

	*dest = 0;
	return dest;
}

/**
 * basic string to float conversion
 */

int str_to_i( const char* a ) {
	return atoi( a );
}

/**
 * basic float to string conversion
 */

float str_to_f( const char* a ) {
	return atof( a );
}


/**
 * convert a single digit to an hex value digit
 */

char xtoa( uint8_t v ) {
	return v < 0x0a ? ( v + '0' ) : ( v - 0x0a + 'A' );
}

/**
 * 
 */

const float muls[] = { 1, 10, 100, 1000, 10000, 100000 };

float round( float v, unsigned ndec, float rnd_spec ) {
	if( ndec>=count_of(muls) ) {
		return v;
	}

	float m = muls[ndec] * rnd_spec;
	v = round( v * m );
	v = v / m;
	return v;
}




Value::Value( cstr s ) {
	value = s;
	buffer[0] = 0;
}

Value::Value( int v ) {
	buffer[0] = 0;
	setInt( v );
}

Value::Value( float v ) {
	buffer[0] = 0;
	setFloat( v );
}

Value::Value( const Value& v ) {
	buffer[0] = 0;
	if( v.value == v.buffer ) {
		memcpy( buffer, v.buffer, sizeof( buffer ) );
		value = buffer;
	}
	else {
		value = v.value;
	}
}

int Value::toInt() const {
	return str_to_i( value );
}

float Value::toFloat() const {
	return str_to_f( value );
}

cstr Value::toStr() const {
	return value;
}

void Value::setStr( cstr s ) {
	value = s;
}

void Value::setInt( int v ) {
	i_to_str( v, buffer );
	value = buffer;
}

void Value::setFloat( float v ) {
	f_to_str( v, 4, buffer );
	value = buffer;
}



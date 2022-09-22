#ifndef __USIS_TOOLS_H
#define __USIS_TOOLS_H

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#if defined( RP2040BM )
#	include "./drivers/rp2040.h"
#elif defined( DESKTOPBM )
#	include "./drivers/desktop.h"
#else
#	include <arduino.h>
#endif

/**
 * usefull shortcuts
 */

typedef const char*   cstr;

/**
 * Tool api
 */

/**
 * check if 2 strings are equal
 */

bool  str_eq( const char* s1, const char* s2 );

/**
 * string to integrer
 */

int   str_to_i( const char* s1 );

/**
 * string to float
 */

float str_to_f( const char* s1 );

/**
 * float to string
 */

char* f_to_str( float number, int digits, char* buffer );

/**
 * int to string
 */

char* i_to_str( int number, char* buffer );

/**
 * convert a nibble to a printable hex digit
 */

char  xtoa( uint8_t v );

/**
 * 
 */

float round( float v, unsigned ndec, float rndv );

/**
 * generic value
 * this class is a small wrapper around a string value
 * and it enable simple conversions
 * 
 * const Value v( "888" );
 * int i = v.toInt();
 */

class Value {
private:
	cstr value;
	char buffer[32 + 1];

public:
	/**
	 * the value of s MUST BE VALID during the life of the value
	 */

	explicit Value( cstr s );
	explicit Value( int v );
	explicit Value( float v );
	Value( const Value& v );

	int toInt() const;
	float toFloat() const;
	cstr toStr() const;

	void setStr( cstr s );
	void setInt( int v );
	void setFloat( float v );
};


#endif
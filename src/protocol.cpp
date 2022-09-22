/**
 * @file protocol.cpp
 * @desc Usis protocol handing
 *
 * @author Etienne Cochard ecochard@r-libre.fr
 * @version 2.0
 **/

#include "protocol.h"

/**
 * constructor
 */

Request::Request( char* parts[5] ) {
	m_command = parts[0];
	m_property = parts[1] ? parts[1] : "";
	m_value1 = parts[2] ? parts[2] : "";
	m_value2 = parts[3] ? parts[3] : "";
}

/**
 * check if the request is the one given
 */

bool Request::is( const char* cmd ) const {
	return str_eq( m_command, cmd );
}

bool Request::is( const char* cmd, const char* prop ) const {
	return str_eq( m_command, cmd ) && str_eq( m_property, prop );
}

bool Request::is( const char* cmd, const char* prop, const char* attr ) const {
	return str_eq( m_command, cmd ) && str_eq( m_property, prop ) && str_eq( m_value1, attr );
}

/**
 * read one value
 */

Value Request::getValue( int index ) const {
	return Value( (const char*)( index == 0 ? m_value1 : m_value2 ) );
}

/**
 *
 */

cstr Request::getValueStr( int index ) const {
	return index == 0 ? m_value1 : m_value2;
}



/**
 * constructor
 */

Response::Response( Stream* stream, bool needCrc ) {
	m_inFrame = false;
	m_done = false;
	m_crc = 0;
	m_stream = stream;
	m_needCrc = needCrc;
}

/**
 * write implementations
 */

void Response::write( uint8_t t ) {
	m_crc ^= t;
	m_stream->write( t );
}

void Response::write( const char* s ) {
	char ch;
	while( ( ch = *s++ ) != 0 ) {
		write( ch );
	}
}

/**
 * send a response
 */

void Response::send( const char* property, const char* attribute, const char* status, const char* value ) {
	_start();

	write( "M00" );
	write( PROTOCOL_SEPARATOR );

	write( property );
	write( PROTOCOL_SEPARATOR );

	write( attribute );

	if( status ) {
		write( PROTOCOL_SEPARATOR );
		write( status );

		if( value ) {
			write( PROTOCOL_SEPARATOR );
			write( value );
		}
	}

	_end();
}

/**
 * send an error
 */

void Response::sendError( const char* code, const char* description ) {
	_start();
	write( code );
	write( PROTOCOL_SEPARATOR );
	write( description );
	_end();
}

/**
 *
 */

void Response::_start() {
	m_crc = 0;
	m_inFrame = true;
}

/**
 *
 */

void Response::_end() {
	m_inFrame = false;

	if( m_needCrc ) {
		char buf[3] = { xtoa( ( m_crc >> 4 ) & 0xf ), xtoa( m_crc & 0xf ), 0 };
		this->write( PROTOCOL_CHECKSUM_SEPARATOR );
		this->write( buf );
	}

	write( PROTOCOL_EOT );
	m_done = true;
}

/**
 * chack if a response has been sent
 */

bool Response::isDone( ) const {
	return m_done;
}

/**
 * Finite State Machine state
 */

struct State
{
	unsigned pos; // current writing position in the buffer
	long time; // start time of the request
	uint8_t crc; // current crc
	bool error; // in error
	int state; // 0: command, 1: property, 2: attribute, 3: value, 4: checksum

	char* parts[5]; // 0: command, 1: property, 2: attribute, 3: value, 4: checksum
					// parts are pointing inside buf
					// NULL means not received

	char buf[PROTOCOL_MAXLEN + 1]; // request buffer;
};

State cState = { 0 };

/**
 * send error result
 */

void comError( Stream* stream, const char* errCode, const char* desc ) {
	Response r( stream, true );
	r.sendError( errCode, desc );
	cState.pos = 0;
}

/**
 * call this fonction the most often possible (inside loop for example)
 * take care of that, arduino implementation of serial buffer is by default
 * 16 bytes wide, so if you do not read chars before it's filled, old chars are lost.
 *
 * it will call handler if a message is received
 */

void processMessages( Stream* stream, pfnMsgHandler handler ) {

	long now = millis();

	int input = stream->read();
	if( input < 0 ) {
		if( cState.pos && ( now - cState.time ) > PROTOCOL_TIMEOUT_MS ) {
			// error: restart
			comError( stream, "C01", "TIMEOUT" );
		}

		return;
	}

	// read next input
	uint8_t ch = (uint8_t)input;
	if( ch=='\r' ) {	// ignore
		return;
	}

	// init cState
	if( cState.pos == 0 ) {
		cState.state = 0;
		cState.time = now;
		cState.crc = 0;
		cState.error = false;
		cState.parts[0] = cState.buf;
		cState.parts[1] = NULL;
		cState.parts[2] = NULL;
		cState.parts[3] = NULL;
		cState.parts[4] = NULL;
	}

	// finite state machine

	// end of transmission
	if( ch == PROTOCOL_EOT ) {

		// ignore empty lines
		if( cState.pos == 0 ) {
			return;
		}

		// are we in error ?
		if( cState.error ) {
			comError( stream, "C04", "OVERFLOW" );
			return;
		}

		// close it
		cState.buf[cState.pos] = 0;

#ifdef DESKTOP_BAREMETAL
		//printf( "[TRACE] cmd: %s, prop: %s, v1: %s, v2: %s\n", cState.parts[0], cState.parts[1], cState.parts[2], cState.parts[3] );
		printf( "> %s;%s;%s;%s\n", cState.parts[0], cState.parts[1], cState.parts[2], cState.parts[3] );
#endif

		// we must have at least command + property, command cannot be empty
		if( !cState.state || *cState.parts[0] == 0 ) {
			comError( stream, "C02", "BAD REQUEST" );
			return;
		}

		// do we have a checksum ?
		if( cState.parts[4] ) {
			const char* checksum = cState.parts[4];
			if( checksum[0] != xtoa( ( cState.crc & 0xf0 ) >> 4 ) || checksum[1] != xtoa( cState.crc & 0xf ) ) {
				comError( stream, "C03", "BAD CHECKSUM" );
				return;
			}
		}

		// everything is ok, call message handler
		Request msg( cState.parts );
		Response rsp( stream, cState.parts[4] ? true : false );

		handler( &msg, &rsp );

		// restart for a new sequence
		cState.pos = 0;
		return;
	}

	// we are in error, just wait EOT
	if( cState.error ) {
		return;
	}

	if( cState.state <= 4 ) {
		// on a separator, skip to next part
		if( ch == PROTOCOL_SEPARATOR ) {
			// in checksum ?
			if( cState.parts[4] ) {
				cState.error = true;
				return;
			}

			cState.buf[cState.pos++] = 0;
			cState.state++;
			cState.parts[cState.state] = &cState.buf[cState.pos];
			cState.crc ^= ch;
		}
		// on the checksum separator
		else if( ch == PROTOCOL_CHECKSUM_SEPARATOR ) {
			// first one ?
			if( cState.parts[4] ) {
				cState.error = true;
				return;
			}

			cState.buf[cState.pos++] = 0;
			cState.parts[4] = &cState.buf[cState.pos];
		}
		// simple char, add it to the buffer (if space available)
		else if( cState.pos < PROTOCOL_MAXLEN ) {
			cState.buf[cState.pos++] = ch;
			if( !cState.parts[4] ) { // no when checksum mark seen
				cState.crc ^= ch;
			}
		}
		// overflow
		else {
			cState.error = true;
		}
	}
	// too many elements
	else {
		cState.error = true;
	}
}

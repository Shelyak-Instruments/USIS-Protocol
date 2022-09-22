/**
 * @file protocol.h
 * @desc Usis protocol handing
 *
 * @author Etienne Cochard ecochard@r-libre.fr
 * @version 1.0
 **/

#ifndef __USIS_PROTOCOL_H
#define __USIS_PROTOCOL_H

#include "./version.h"
#include "./tools.h"

// max length of a received message (end of line included)
#define PROTOCOL_MAXLEN 150

// max length of a response message
#define PROTOCOL_MAX_RESP_LEN 255

// elements separator
#define PROTOCOL_SEPARATOR ';'

// timeout
#define PROTOCOL_TIMEOUT_MS 1000

// checksum separator
#define PROTOCOL_CHECKSUM_SEPARATOR '*'

// end of message
#define PROTOCOL_EOT '\n'

// forward references
class Request;
class Response;
class WorkingBuffer;

// prototype of a message handler
typedef void ( *pfnMsgHandler )( Request*, Response* );

/**
 * Request class
 */

class Request {

private:
	// different parts received
	const char* m_command;
	const char* m_property;
	const char* m_value1;
	const char* m_value2;

public:
	// parts must keep alive during the life of the Request object
	Request( char* parts[5] );

	//
	bool is( const char* cmd ) const;
	bool is( const char* cmd, const char* prop ) const;
	bool is( const char* cmd, const char* prop, const char* attr ) const;

	const char* getCommand() const {
		return m_command;
	}

	const char* getProperty() const {
		return m_property;
	}

	const char* getAttr() const {
		return m_value1;
	}

	Value getValue( int index ) const;
	cstr getValueStr( int index ) const;
};

/**
 * Response class
 */

class Response {

private:
	uint8_t m_crc;	// current running crc
	Stream* m_stream; // stream on we are writing
	
	bool m_inFrame;	// we are in frame (compute crc on written elements)
	bool m_needCrc; // do we need to send crc
	bool m_done;	// a response was sent

public:
	explicit Response( Stream* stream, bool needCrc );

	void send( const char* property, const char* attribute, const char* status, const char* value );
	void sendError( const char* code, const char* desc );

	bool isDone( ) const;

private:
	// write implementation
	void write( uint8_t t );
	void write( const char* s );

	void _start();
	void _end();
};

/**
 * process message
 * call this as often you can
 */

void processMessages( Stream* serial, pfnMsgHandler handler );

#endif
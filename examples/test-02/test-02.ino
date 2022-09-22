#include <Usis.h>

PROPERTIES_START( ) 

	PROPERTY_START( "GRATING_ANGLE", PROPERTY_TYPE_FLOAT, 0.0f, NULL )
		PROPERTY_ATTR( "MIN", PROPERTY_TYPE_FLOAT|PROPERTY_FLAG_READONLY, 0.0f )
		PROPERTY_ATTR( "MAX", PROPERTY_TYPE_FLOAT|PROPERTY_FLAG_READONLY, 360.0f )
		PROPERTY_ATTR( "UNIT", PROPERTY_TYPE_CSTR|PROPERTY_FLAG_READONLY, "DEGREE" )
	PROPERTY_END( )

	PROPERTY_START( "FOCUS_POSITION", PROPERTY_TYPE_FLOAT, 0.0f, NULL )
		PROPERTY_ATTR( "MIN", PROPERTY_TYPE_FLOAT|PROPERTY_FLAG_READONLY, 0.0f )
		PROPERTY_ATTR( "MAX", PROPERTY_TYPE_FLOAT|PROPERTY_FLAG_READONLY, 360.0f )
		PROPERTY_ATTR( "UNIT", PROPERTY_TYPE_CSTR|PROPERTY_FLAG_READONLY, "DEGREE" )
	PROPERTY_END( )

	PROPERTY_START( "LIGHT_SOURCE", PROPERTY_TYPE_ENUM, 0, NULL, "AAA", "BBB", "CCC" )
	PROPERTY_END( )

PROPERTIES_END( );

// the setup function runs once when you press reset or power the board
void setup() {
  	Serial.begin( 9600 );  
	pinMode(LED_BUILTIN, OUTPUT);
}

// handle messages received on the Usis protocol handler
void handleMessage( Request* req, Response* res ) {

	// just toggle the led
	digitalWrite(LED_BUILTIN, HIGH );

	if( processProperty( req, res )==1 ) {
		res->sendError( "M01", "UNKNOWN COMMAND" );
	}

	digitalWrite(LED_BUILTIN, LOW );
}

// the loop function runs over and over again forever
void loop() {
  	processMessages( &Serial, handleMessage );
}



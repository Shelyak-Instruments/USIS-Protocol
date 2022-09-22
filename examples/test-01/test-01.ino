#include <Usis.h>

// the setup function runs once when you press reset or power the board
void setup() {
  	Serial.begin( 9600 );  
	pinMode(LED_BUILTIN, OUTPUT);
}

// handle messages received on the Usis protocol handler
void handleMessage( Request* req, Response* rsp ) {

	// just toggle the led
	digitalWrite(LED_BUILTIN, HIGH );
	digitalWrite(LED_BUILTIN, HIGH );

	if( req->is( "GET", "VERSION" ) ) {
		rsp->send( "VERSION", "1.0.0", "", "" );
	}
	else {
		// send an error
		rsp->sendError( "M99", "COMMAND NOT HANDLED" );
	}

	digitalWrite(LED_BUILTIN, LOW );
}

// the loop function runs over and over again forever
void loop() {
  	processMessages( &Serial, handleMessage );
}



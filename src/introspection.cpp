/**
 * @file introspection.cpp
 * @desc Usis instrospection message handing
 *
 * @author Etienne Cochard ecochard@r-libre.fr
 * @version 1.0
 **/

#include <stdarg.h>
#include "introspection.h"

/*
	INFO;PROPERTY_COUNT return INT the device property count
	INFO;PROPERTY_NAME;<prop_num> return TEXT the device property name (prop_num starts at 0)
	INFO;PROPERTY_TYPE;<prop_num> return TEXT the device property type
	INFO;PROPERTY_ATTR_COUNT;<prop_num> return INT property attribute count
	INFO;PROPERTY_ATTR_NAME;<prop_num>;<attr_num> return TEXT <attribute name>
	INFO;PROPERTY_ATTR_TYPE;<prop_num>;<attr_num> return TEXT <attribute type>,<read only>
	INFO;PROPERTY_ATTR_STATE;<prop_num>;<attr_num> return TEXT <attribute state>
	INFO;PROPERTY_ATTR_ENUM_COUNT;<prop_num> return INT <Nb of possible values>
	INFO;PROPERTY_ATTR_ENUM_VALUE;<prop_num>;<enum_num> return TEXT <value text>
*/



/**
 * check if property is a command
 */

bool isCommand( rawProperty* p ) {
	if( !p->attrs ) {
		return false;
	}
	
	rawAttribute* a = &p->attrs[0];
	const uint8_t type = a->value.attrs & PROPERTY_TYPE_MASK;
		
	return type&PROPERTY_TYPE_CMD ? true : false;
}

/**
 * return the nth property
 */

rawProperty* getPropertyByIndex( int idx, bool skipCmd ) {
	rawProperty* p = properties;
	int count = 0;

	while( p ) {

		if( !skipCmd || !isCommand(p) ) {
			if( count == idx ) {
				return p;
			}

			count++;
		}

		p = p->next;
	}

	return NULL;
}

/**
 * return the nth attribute
 */

rawAttribute* getAttributeByIndex( rawProperty* p, int idx ) {
	rawAttribute* a = p->attrs;
	int count = 0;

	while( a ) {
		if( count == idx ) {
			return a;
		}

		count++;
		a = a->next;
	}

	return NULL;
}

/**
 * 
 */

bool getReqIntAttr( Request* req, int rval, int* result ) {
	cstr sid = req->getValueStr( rval );
	if( !isValidNumber( sid, false ) ) {
		return false;
	}

	*result = str_to_i( sid );
	return true;
}

/**
 * 
 */

cstr getAttrTypeText( uint8_t attrs ) {
	const uint8_t type = attrs & PROPERTY_TYPE_MASK;

	switch( type ) {
		case PROPERTY_TYPE_CMD:
			return "COMMAND";
		case PROPERTY_TYPE_CSTR:
			return "TEXT";
		case PROPERTY_TYPE_ENUM:
			return "ENUM";
		case PROPERTY_TYPE_FLOAT:
			return "FLOAT";
		case PROPERTY_TYPE_INT:
			return "INT";
	}

	return "";
}

/**
 * 
 */

cstr getAttrModeText( uint8_t attrs ) {
	
	if( attrs&PROPERTY_FLAG_READONLY ) {
		return "RO";
	}
	else {
		return "RW";
	}
}

/**
 * 
 */

cstr getAttrStateText( uint8_t attrs ) {
	const uint8_t type = attrs & PROPERTY_STATE_MASK;
	switch( type ) {
		case PROPERTY_STATE_READY:
			return "OK";
		case PROPERTY_STATE_BUSY:
			return "BUSY";
		case PROPERTY_STATE_ALERT:
			return "ALERT";
		case PROPERTY_STATE_IDLE:
			return "IDLE";
		case PROPERTY_STATE_NA:
			return "NA";
	}

	return "";
}

/**
 * 
 */

int processIntrospection( Request* req, Response* res ) {

	cstr prop = req->getProperty();

	/**
	 * COUNT
	 */

	if( str_eq( prop, "PROPERTY_COUNT" ) ) {
		rawProperty* p = properties;
		int count = 0;

		while( p ) {
			if( !isCommand(p) ) {
				count++;
			}

			p = p->next;
		}

		res->send( prop, "", "OK", Value( count ).toStr() );
		return 0;
	}

	/**
	 * NAME
	 */

	if( str_eq( prop, "PROPERTY_NAME" ) ) {
		int idx = 0;
		if( getReqIntAttr( req, 0, &idx ) ) {
			rawProperty* p = getPropertyByIndex( idx, true );
			if( p ) {
				res->send( prop, "", "OK", p->name );
				return 0;
			}
		}

		res->sendError( "M09", "BAD INDEX" );
		return -1;
	}

	/**
	 * TYPE
	 */

	if( str_eq( prop, "PROPERTY_TYPE" ) ) {
		int propIdx = 0;
		if( getReqIntAttr( req, 0, &propIdx ) ) {
			rawProperty* p = getPropertyByIndex( propIdx, true );
			if( p ) {
				res->send( prop, "", "OK", getAttrTypeText( p->attrs[0].value.attrs ) );
				return 0;
			}
		}

		res->sendError( "M09", "BAD INDEX" );
		return -1;
	}
	
	/**
	 * STATE
	 */

	if( str_eq( prop, "PROPERTY_STATE" ) ) {
		int propIdx = 0;
		if( getReqIntAttr( req, 0, &propIdx ) ) {

			rawProperty* p = getPropertyByIndex( propIdx, true );
			if( p && p->attrs ) {
				rawAttribute* a = &p->attrs[0];
				res->send( prop, "", "OK", getAttrStateText( a->value.attrs ) );
				return 0;
			}
		}

		res->sendError( "M09", "BAD INDEX" );
		return -1;
	}
	
	/**
	 * ATTR COUNT
	 */

	if( str_eq( prop, "PROPERTY_ATTR_COUNT" ) ) {
		int idx = 0;
		if( getReqIntAttr( req, 0, &idx ) ) {
			rawProperty* p = getPropertyByIndex( idx, true );
			if( p ) {
				int count = 0;
				rawAttribute* a = p->attrs;

				while( a ) {
					count++;
					a = a->next;
				}

				res->send( prop, "", "OK", Value( count ).toStr() );
				return 0;
			}
		}

		res->sendError( "M09", "BAD INDEX" );
		return -1;
	}
	
	/**
	 * ATTR NAME
	 */

	if( str_eq( prop, "PROPERTY_ATTR_NAME" ) ) {
		int propIdx = 0;
		int attrIdx = 0;
		if( getReqIntAttr( req, 0, &propIdx ) && getReqIntAttr( req, 1, &attrIdx ) ) {

			rawProperty* p = getPropertyByIndex( propIdx, true );
			if( p ) {
				rawAttribute* a = getAttributeByIndex( p, attrIdx );
				if( a ) {
					res->send( prop, "", "OK", a->name );
					return 0;
				}
			}
		}

		res->sendError( "M09", "BAD INDEX" );
		return -1;
	}
	
	/**
	 * ATTR TYPE
	 */

	if( str_eq( prop, "PROPERTY_ATTR_MODE" ) ) {
		int propIdx = 0;
		int attrIdx = 0;
		if( getReqIntAttr( req, 0, &propIdx ) && getReqIntAttr( req, 1, &attrIdx ) ) {

			rawProperty* p = getPropertyByIndex( propIdx, true );
			if( p ) {
				rawAttribute* a = getAttributeByIndex( p, attrIdx );
				if( a ) {
					res->send( prop, "", "OK", getAttrModeText( a->value.attrs ) );
					return 0;
				}
			}
		}

		res->sendError( "M09", "BAD INDEX" );
		return -1;
	}
	
	/**
	 * ENUM COUNT
	 */

	if( str_eq( prop, "PROPERTY_ATTR_ENUM_COUNT" ) ) {
		int propIdx = 0;
		int attrIdx = 0;

		if( getReqIntAttr( req, 0, &propIdx ) && getReqIntAttr( req, 1, &attrIdx ) ) {
			rawProperty* p = getPropertyByIndex( propIdx, true );
			if( p ) {
				rawAttribute* a = getAttributeByIndex( p, attrIdx );
				if( a && ( a->value.attrs & PROPERTY_TYPE_MASK ) == PROPERTY_TYPE_ENUM ) {
					res->send( prop, "", "OK", Value(a->value.ecount).toStr() );
					return 0;
				}
			}
		}

		res->sendError( "M09", "BAD INDEX" );
		return -1;
	}
	
	/**
	 * ENUM VALUES
	 */

	if( str_eq( prop, "PROPERTY_ATTR_ENUM_VALUE" ) ) {
		int propIdx = 0;
		
		if( getReqIntAttr( req, 0, &propIdx ) ) {
			rawProperty* p = getPropertyByIndex( propIdx, true );
			if( p ) {
				Value sid = req->getValue( 1 );
				int enumIdx = sid.toInt( );

				rawAttribute* a = getAttributeByIndex( p, 0 );
				if( a && ( a->value.attrs & PROPERTY_TYPE_MASK ) == PROPERTY_TYPE_ENUM && enumIdx>=0 && enumIdx<a->value.ecount ) {
					res->send( prop, "VALUE", "OK", a->value.evals[enumIdx] );
					return 0;
				}
			}
		}

		res->sendError( "M09", "BAD INDEX" );
		return -1;
	}

	res->sendError( "M01", "UNKNOWN PROPERTY" );
	return -1;
}

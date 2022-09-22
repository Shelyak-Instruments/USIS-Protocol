/**
 * @file properties.cpp
 * @desc Usis properties handing
 *
 * @author Etienne Cochard ecochard@r-libre.fr
 * @version 1.0
 **/

#include <stdarg.h>
#include "properties.h"
#include "introspection.h"


/**
 * add a property to the global properties
 */

void addProperty( rawProperty* p ) { 

	p->next = NULL;

	if( !properties ) {
		properties = p;
	}
	else {
		rawProperty* pp = properties;
		while( pp->next ) {
			pp = pp->next;
		}

		pp->next = p;
	}
}

void addAttribute( rawProperty* p, rawAttribute* a ) {
	
	if( !p->attrs ) {
		p->attrs = a;
	}
	else {
		rawAttribute* pa = p->attrs;
		while( pa->next ) {
			pa = pa->next;
		}

		pa->next = a;
	}
}


/**
 * initialize the property
 */

void __makeProperty( rawProperty* prop, cstr name, pfnHandler chg ) {
	addProperty( prop );
	memset( prop, 0, sizeof(prop) );
	prop->name = name;
	prop->handler = chg;
}

/**
 * add an attribute to the given property
 */

static const cstr __value = "VALUE";				

void __addAttribute( rawProperty* prop, rawAttribute* pattr, cstr name, unsigned attr, const __uv& v, int ecount, cstr* enums, pfnHandler handler ) {
	pattr->name = name ? name : __value;
	pattr->id = 0;
	pattr->value.attrs = attr;
	pattr->value.fval = v.fval;	// sizeof(float) >= sizeof(char*) 
	pattr->value.ecount = ecount;
	pattr->value.evals = enums;
	pattr->next = NULL;
	pattr->handler = handler;

	addAttribute( prop, pattr );
}



/**
 *
 */

int set_variant( rawValue* var, float v ) {

	// check !readonly
	if( var->attrs & PROPERTY_FLAG_READONLY ) {
		return -1;
	}

	const uint8_t type = var->attrs & PROPERTY_TYPE_MASK;
	if( type != PROPERTY_TYPE_FLOAT ) {

		// int set in float
		if( type == PROPERTY_TYPE_INT ) {
			var->fval = (float)v;
			return 0;
		}

		return -2;
	}

	// ok, can set value
	var->fval = v;
	return 0;
}

/**
 *
 */

int set_variant( rawValue* var, int v ) {

	// check !readonly
	if( var->attrs & PROPERTY_FLAG_READONLY ) {
		return -1;
	}

	const uint8_t type = var->attrs & PROPERTY_TYPE_MASK;
	if( type==PROPERTY_TYPE_ENUM ) {
		// todo: check enum value
	}
	else if( type != PROPERTY_TYPE_INT ) {

		// flaot set in int
		if( type == PROPERTY_TYPE_FLOAT ) {
			var->ival = (int)v; // loose precision
			return 0;
		}

		return -2;
	}

	// ok, can set value
	var->ival = v;
	return 0;
}

/**
 *
 */

int set_variant( rawValue* var, cstr v ) {
	// check !readonly
	if( var->attrs & PROPERTY_FLAG_READONLY ) {
		return -1;
	}

	const uint8_t type = var->attrs & PROPERTY_TYPE_MASK;

	if( type== PROPERTY_TYPE_ENUM ) {
		for( int i=0; i<var->ecount; i++ ) {
			cstr p = var->evals[i];
			if( str_eq(p,v) ) {
				var->ival = i;
				return 0;
			}
		}
		
		return -3;
	}
	else if( type != PROPERTY_TYPE_CSTR ) {

		// char* set in int
		if( type == PROPERTY_TYPE_INT ) {
			var->ival = atoi( v );
			return 0;
		}

		// char* set in float
		if( type == PROPERTY_TYPE_FLOAT ) {
			var->fval = atof( v );
			return 0;
		}

		return -2;
	}

	// ok, can set value
	var->sval = v;
	return 0;
}

/**
 * change the variant state
 * @param var the variant to change
 * @param state new state
 */

void set_variant_state( rawValue* var, uint8_t state ) {
	var->attrs &= ~PROPERTY_STATE_MASK;
	var->attrs |= state & ~PROPERTY_STATE_MASK;
}

/**
 * search for a property in all defined properties
 * @param name - the property we are looking for
 * @returns the property or NULL
 *
 * @example
 * 	rawProperty* prop = findProperty( "GRATING_ANGLE" );
 */

rawProperty* findProperty( cstr name ) {

	rawProperty* p = properties;
	while( p ) {
		if( str_eq( p->name, name ) ) {
			return p;
		}
		p = p->next;
	}

	return NULL;
}

/**
 * search for an attribute in the property
 * @param prop - the property inside which we look
 * @param attrName - the name we are looking at
 * @return the attribute or NULL if not found
 *
 * @example
 * 	rawPropertyAttr* attr = findAttr( prop, "MIN" );
 */

rawAttribute* findAttr( rawProperty* prop, cstr attrName ) {

	rawAttribute* pa = prop->attrs;
	while( pa ) {
		if( str_eq(pa->name,attrName) ) {
			return pa;
		}

		pa = pa->next;
	}

	return NULL;
}

/**
 * change an attribute value (float version)
 * @param attr - attribute we want to change
 * @param v - new value
 * @return 0 if ok
 * 			-1 if readonly
 * 			-2 if bad type
 */

int setAttr( rawAttribute* a, float v ) {
	return set_variant( &a->value, v );
}

/**
 * change an attribute value (int version)
 * @param attr - attribute we want to change
 * @param v - new value
 * @return 0 if ok
 * 			-1 if readonly
 * 			-2 if bad type
 */

int setAttr( rawAttribute* a, int v ) {
	return set_variant( &a->value, v );
}

/**
 * change an attribute value (char* version)
 * @param attr - attribute we want to change
 * @param v - new value
 * @return 0 if ok
 * 			-1 if readonly
 * 			-2 if bad type
 */

int setAttr( rawAttribute* a, cstr v ) {
	return set_variant( &a->value, v );
}

/**
 * change the attribute state
 * @param attr attribute to change
 * @param state new state
 */

void setAttrState( rawAttribute* a, uint8_t state ) {
	set_variant_state( &a->value, state );
}

/**
 * change the property value (float version)
 * @param prop - the property to change
 * @param v - new value
 * @return 0 if ok
 * 			-1 if readonly
 * 			-2 if bad type
 * @example
 * 	setPropertyValue( prop, 360.0f );
 */

int setPropertyValue( rawProperty* prop, float v ) {
	return setAttr( &prop->attrs[0], v );
}

/**
 * change the property value (int version)
 * @param prop - the property to change
 * @param v - new value
 * @return 0 if ok
 * 			-1 if readonly
 * 			-2 if bad type
 * @example
 * 	setPropertyValue( prop, 360 );
 */

int setPropertyValue( rawProperty* prop, int v ) {
	return setAttr( &prop->attrs[0], v );
}

/**
 * change the property value (char* version)
 * @param prop - the property to change
 * @param v - new value
 * @return 0 if ok
 * 			-1 if readonly
 * 			-2 if bad type
 * @example
 * 	setPropertyValue( prop, "DEGREE" );
 */

int setPropertyValue( rawProperty* prop, cstr v ) {
	return setAttr( &prop->attrs[0], v );
}

/**
 * change the property value state
 * @param prop property to change
 * @param state new state cf. PROPERTY_STATE_<xxx>
 * @return
 */

void setPropertyState( rawProperty* prop, uint8_t state ) {
	setAttrState( &prop->attrs[0], state );
}

/**
 * 	compute property state string
 */

cstr calcPropState( uint8_t state ) {
	switch( state & PROPERTY_STATE_MASK ) {
		case PROPERTY_STATE_BUSY: {
			return "BUSY";
		}
		case PROPERTY_STATE_ALERT: {
			return "ALERT";
		}
		case PROPERTY_STATE_IDLE: {
			return "IDLE";
		}
		case PROPERTY_STATE_NA: {
			return "NA";
		}
	}

	return "OK"; // todo: ready
}

/**
 * convert the given value to string
 */

cstr valueToStr( rawValue* var, char* buffer ) {

	*buffer = 0;
	switch( var->attrs & PROPERTY_TYPE_MASK ) {
		case PROPERTY_TYPE_INT: {
			i_to_str( var->ival, buffer );
			break;
		}

		case PROPERTY_TYPE_FLOAT: {
			f_to_str( var->fval, 4, buffer );
			break;
		}

		case PROPERTY_TYPE_ENUM: {
			//todo: check range.
			return var->evals[var->ival];
		}

		case PROPERTY_TYPE_CSTR: {
			return var->sval;
		}
	}

	return buffer;
}

/**
 * check if the value is a valid number
 * @param v string 0 term
 * @param flt true if float values accepted, false means only integers
 */

bool isValidNumber( cstr v, bool flt ) {
	const char* p = v;

	// empty strings are refused
	if( *p==0 ) {
		return false;
	}

	bool dot = false;
	char ch;
	
	// allow negative values
	if( *p=='-' ) {
		p++;
	}

	// scan chars
	while( (ch=*p)!=0 ) {
	
		if( ch<'0' || ch>'9' ) {

			// not a number
			if( !flt || ch!='.' || dot ) {
				return false;
			}

			// but '.' and we allow floats
			dot = true;
		}

		p++;
	}

	// do not accept "." (len=1 && seen a dot)
	if( (p-v)==1 && dot ) {
		return false;
	}

	return true;
}


/**
 * handle GET command
 */

int processPropertyGet( Request* req, Response* res ) {
	rawProperty* prop = findProperty( req->getProperty() );
	if( !prop ) {
		res->sendError( "M01", "UNKNOWN PROPERTY" );
		return -1;
	}

	rawAttribute* attr = findAttr( prop, req->getValueStr( 0 ) );
	if( !attr ) {
		res->sendError( "M02", "UNKNOWN ATTRIBUTE" );
		return -1;
	}

	if( prop->handler && attr->id==0  ) {
		prop->handler( MsgGet, req, res, &attr->value );
	}

	if( !res->isDone() ) {
		static char buffer[32];
		res->send( prop->name, attr->name, calcPropState( attr->value.attrs ), valueToStr( &attr->value, buffer ) );
		return 1;
	}
	
	return 0;
}



/**
 * handle SET command
 */

int processPropertySet( Request* req, Response* res ) {

	// get property
	rawProperty* prop = findProperty( req->getProperty() );
	if( !prop ) {
		res->sendError( "M01", "UNKNOWN PROPERTY" );
		return -1;
	}

	// search given attribute
	rawAttribute* attr = findAttr( prop, req->getValueStr( 0 ) );
	if( !attr ) {
		res->sendError( "M02", "UNKNOWN ATTRIBUTE" );
		return -1;
	}

	// get value & convert to the correct type
	int rc = -2;
	cstr v = req->getValueStr( 1 );
	if( !v || *v==0 ) {
		res->sendError( "M05", "NO VALUE GIVEN" );
		return -1;
	}

	switch( attr->value.attrs & PROPERTY_TYPE_MASK ) {
		case PROPERTY_TYPE_INT: {
			// check all chars are digits
			if( !isValidNumber(v,false) ) {
				break;
			}
			
			rc = setAttr( attr, str_to_i(v) );
			break;
		}

		case PROPERTY_TYPE_FLOAT: {
			if( !isValidNumber(v,true) ) {
				break;
			}
			
			rc = setAttr( attr, str_to_f(v) );
			break;
		}

		case PROPERTY_TYPE_ENUM: {
			rc = setAttr( attr, v );
			break;
		}

		case PROPERTY_TYPE_CSTR: {
			rc = setAttr( attr, v );
			break;
		}
	}

	switch( rc ) {
		case -1: {
			res->sendError( "M03", "READONLY" );
			return -1;
		}

		case -2: {
			res->sendError( "M04", "BAD VALUE TYPE" );
			return -1;
		}

		case -3: {
			res->sendError( "M08", "BAD VALUE" );
			return -1;
		}
	}

	if( prop->handler && attr->id==0 ) {
		prop->handler( MsgSet, req, res, &attr->value );
	}

	if( !res->isDone() ) {
		static char buffer[32];
		res->send( prop->name, attr->name, calcPropState( attr->value.attrs ), valueToStr( &attr->value, buffer ) );
		return 1;
	}

	return 0;
}

/**
 * process message according to the properties defined in the application
 *
 * @return -1 in case of error
 * 			0 in case of success
 * 			1 in case of command not handled
 */

int processProperty( Request* req, Response* res ) {

	if( req->is( "GET" ) ) {
		return processPropertyGet( req, res );
	}
	else if( req->is( "SET" ) ) {
		return processPropertySet( req, res );
	}
	else if( req->is( "INFO" ) ) {
		return processIntrospection( req, res );
	}
	else {
		cstr cmd = req->getCommand( );
		rawProperty* p = findProperty( cmd );

		if( p ) {
			if( p->handler ) {
				p->handler( MsgCmd, req, res, NULL );
			}
			else {
				cstr prop = req->getProperty( );
				rawAttribute* a = findAttr( p, prop );

				if( a && a->handler ) {
					a->handler( MsgCmd, req, res, NULL );
				}
			}
		}

		if( !res->isDone() ) {
			res->sendError( "M06", "UNKNOWN COMMAND");
			return 1;
		}
	}

	return 0;
}
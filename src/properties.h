/**
 * @file properties.h
 * @desc Usis properties handing
 *
 * @author Etienne Cochard ecochard@r-libre.fr
 * @version 1.0
 **/

#ifndef __USIS_PROPERTIES_H
#define __USIS_PROPERTIES_H

#include "tools.h"
#include "protocol.h"

/**
 * VARIANTS
 * see PROPERTIES_START for usage
 */

#define PROPERTY_TYPE_INT 0 // ival
#define PROPERTY_TYPE_FLOAT 1 // fval
#define PROPERTY_TYPE_ENUM 2 // sval
#define PROPERTY_TYPE_CSTR 3 // sval
#define PROPERTY_TYPE_CMD 4 // -
#define PROPERTY_TYPE_MASK 0b00000111 // mask to extract the property type

#define PROPERTY_STATE_READY ( 0 << 3 )
#define PROPERTY_STATE_BUSY ( 1 << 3 )
#define PROPERTY_STATE_ALERT ( 2 << 3 )
#define PROPERTY_STATE_IDLE ( 3 << 3 )
#define PROPERTY_STATE_NA ( 4 << 3 )
#define PROPERTY_STATE_MASK 0b00111000 // mask to extract the state type

#define PROPERTY_FLAG_READONLY 0b10000000 // the propertty is readonly

/**
 * internal, helper to store integer, float or char* value
 */

union __uv {
	int ival;
	float fval;
	cstr sval;
	
	__uv( int v ) {
		ival = v;
	}
	__uv( float v ) {
		fval = v;
	}
	__uv( cstr v ) {
		sval = v;
	}
};

/**
 * a simple variant holder
 * CARE: when storing char*, the value is not copied.
 * source MUST be valid for the life of the variant
 */

#pragma pack( push )
struct rawValue
{
	union {
		int ival; // if type == Int
		float fval; // if type == Float
		cstr sval; // if type == Str | Enum
	};

	uint8_t attrs; // PROPERTY_TYPE_<xxx> | PROPERTY_STATE_<xxx> | PROPERTY_FLAG_<xxx>
	uint8_t ecount; // count of enums
	cstr* evals; // possible enum values NULL term
};
#pragma pack( pop )

int set_variant( rawValue* var, float v );
int set_variant( rawValue* var, int v );
int set_variant( rawValue* var, cstr v );

enum PropertyMsg
{
	MsgSet = 1,
	MsgGet = 2,
	MsgCmd = 3,
};

//	function prototype when a value is changing
typedef void ( *pfnHandler )( PropertyMsg msg, Request* req, Response* res, rawValue* value );

#define NULL_TERM( ... ) \
	{ ##__VA_ARGS__, NULL }

#define PROPERTIES_START() rawProperty* __makeProps() {

// CARE: ival mut match the property type
// ie. 	if property type is float, ival must of type float
//		if property type is enum, ival must be of type int
#define PROPERTY_START( name, attr, ival, handler, ... ) \
	{ \
		static rawProperty p; \
		static int8_t uid = 0; \
		static rawAttribute pv; \
		{ \
			static const cstr _name = name; \
			static const cstr e[] = { __VA_ARGS__ }; \
			__makeProperty( &p, _name, handler ); \
			__addAttribute( &p, &pv, NULL, attr, ival, count_of( e ), (cstr*)e, NULL ); \
			pv.id = uid++; \
		}

#define PROPERTY_ATTR( name, attr, ival, ... ) \
	{ \
		static rawAttribute pv; \
		static const cstr _name = name; \
		static const cstr e[] = { __VA_ARGS__ }; \
		__addAttribute( &p, &pv, _name, attr, ival, count_of( e ), (cstr*)e, NULL ); \
		pv.id = uid++; \
	}

#define PROPERTY_END() \
	}

#define COMMAND_START( name ) \
	{ \
		static rawProperty p; \
		{ \
			static const cstr _name = name; \
			__makeProperty( &p, _name, NULL ); \
		} 

#define COMMAND_HANDLER( name, handler ) \
	{ \
		static rawAttribute pv; \
		__addAttribute( &p, &pv, name, PROPERTY_TYPE_CMD, 0, 0, NULL, handler ); \
	}		
		
#define COMMAND_END() \
	}


#define PROPERTIES_END() \
	return properties; \
	} \
	rawProperty* properties = __makeProps();

/**
 * raw definition of a property attribute
 */

struct rawAttribute
{
	cstr name; // attr name
	uint8_t id; // attr id
	rawValue value; // attr value
	pfnHandler handler; // attribute callback
	rawAttribute* next; // next in list, NULL for last
};

/**
 * raw definition of a property
 * by default, a property has always an attribute name "VALUE"
 * other attributes can be added
 */

struct rawProperty
{
	cstr name; // property name
	pfnHandler handler; // property callback
	rawAttribute* attrs; // chained attributes
	rawProperty* next; // next in list, NULL for last
};

/**
 * global properties
 */

extern rawProperty* properties;

void __makeProperty( rawProperty* prop, cstr name, pfnHandler hanlder );
void __addAttribute( rawProperty* prop, rawAttribute* pattr, cstr name, unsigned attr, const __uv& v, int nenum, cstr* enums, pfnHandler handler );

/**
 * search for a property in all defined properties
 * @param name - the property we are looking for
 * @returns the property or NULL
 *
 * @example
 * 	rawProperty* prop = findProperty( "GRATING_ANGLE" );
 */

rawProperty* findProperty( cstr name );

/**
 * search for an attribute in the property
 * @param prop - the property into we need to look
 * @param attrName - the name we are looking at
 * @return the attribute or NULL if not found
 *
 * @example
 * 	rawPropertyAttr* attr = findAttr( prop, "MIN" );
 */

rawAttribute* findAttr( rawProperty* prop, cstr attrName );

/**
 * change an attribute value (float version)
 * @param attr - attribute we want to change
 * @param v - new value
 * @return 0 if ok
 * 			-1 if readonly
 * 			-2 if bad type
 */

int setAttr( rawAttribute* a, float v );

/**
 * change an attribute value (int version)
 * @param attr - attribute we want to change
 * @param v - new value
 * @return 0 if ok
 * 			-1 if readonly
 * 			-2 if bad type
 */

int setAttr( rawAttribute* a, int v );

/**
 * change an attribute value (char* version)
 * @param attr - attribute we want to change
 * @param v - new value
 * @return 0 if ok
 * 			-1 if readonly
 * 			-2 if bad type
 */

int setAttr( rawAttribute* a, cstr v );

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

int setPropertyValue( rawProperty* prop, float v );

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

int setPropertyValue( rawProperty* prop, int v );

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

int setPropertyValue( rawProperty* prop, cstr v );

/**
 * change the property attribute value (float version)
 * @param prop - the property to change
 * @param v - new value
 * @return 0 if ok
 * 			-1 if readonly
 * 			-2 if bad type
 * 			-3 if unknown attribute
 * @example
 * 	setPropertyValue( prop, "MAX", 360.0f );
 */

int setPropertyValue( rawProperty* prop, cstr attrName, float v );

/**
 * change the property value (float version)
 * @param prop - the property to change
 * @param v - new value
 * @return 0 if ok
 * 			-1 if readonly
 * 			-2 if bad type
 * 			-3 if unknown attribute
 * @example
 * 	setPropertyValue( prop, "MAX", 360 );
 */

int setPropertyValue( rawProperty* prop, cstr attrName, int v );

int setPropertyValue( rawProperty* prop, cstr attrName, const rawValue& v );

/**
 * change the property attribute value (char* version)
 * @param prop - the property to change
 * @param v - new value
 * @return 0 if ok
 * 			-1 if readonly
 * 			-2 if bad type
 * 			-3 if unknown attribute
 * @example
 * 	setPropertyValue( prop, "UNIT", "DEGREE" );
 */

int setPropertyValue( rawProperty* prop, cstr attrName, cstr v );

/**
 * change the property state
 * cf. PROPERTY_STATE_xxx
 */

void setPropertyState( rawProperty* props, uint8_t state );

/**
 * change the property state
 * cf. PROPERTY_STATE_xxx
 */

bool setPropertyState( rawProperty* props, cstr attrName, uint8_t state );

/**
 * process message according to the properties defined in the application
 *
 * @return -1 in case of error
 * 			0 in case of success
 * 			1 in case of command not handled
 */

int processProperty( Request* req, Response* res );

bool isValidNumber( cstr v, bool flt );

#endif

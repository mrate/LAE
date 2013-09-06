#include "Xml.h"

namespace LAE {
namespace xml {

// save
template<>
void saveAttribute<const std::string&>( TiXmlElement* element, const char* name, const std::string& value ) {
	TiXmlElement* attrE = new TiXmlElement( name );
	attrE->LinkEndChild( new TiXmlText( value.c_str() ) );
	element->LinkEndChild( attrE );
}

template<>
void saveAttribute<bool>( TiXmlElement* element, const char* name, bool value ) {
	TiXmlElement* attrE = new TiXmlElement( name );
	attrE->LinkEndChild( new TiXmlText( value ? "true" : "false" ) );
	element->LinkEndChild( attrE );
}

template<>
void saveAttribute<unsigned char>( TiXmlElement* element, const char* name, unsigned char value ) {
	TiXmlElement* attrE = new TiXmlElement( name );
	attrE->LinkEndChild( new TiXmlText( boost::lexical_cast<std::string>( (int)value ).c_str() ) );
	element->LinkEndChild( attrE );
}

// load mandatory
template<>
std::string loadAttribute<std::string>( const TiXmlElement* element, const char* name ) {
	const TiXmlElement* child = element->FirstChildElement(name);
	if( child == nullptr ) {
		throw LAEException( "Missing element " ) << name;
	}
	const char* value = child->GetText();
	return value ? value : std::string();
}

template<>
bool loadAttribute<bool>( const TiXmlElement* element, const char* name ) {
	const TiXmlElement* child = element->FirstChildElement(name);
	if( child == nullptr ) {
		throw LAEException( "Missing element " ) << name;
	}
	const char* value = child->GetText();	
	return value ? strcmp(value, "true") == 0 : false;
}

// load optional
template<>
std::string loadAttribute<std::string>( const TiXmlElement* element, const char* name, const std::string& defaultValue ) {
	const TiXmlElement* child = element->FirstChildElement(name);
	if( child == nullptr ) {
		return defaultValue;
	}
	const char* value = child->GetText();
	return value ? value : std::string();
}

template<>
bool loadAttribute<bool>( const TiXmlElement* element, const char* name, bool defaultValue ) {
	const TiXmlElement* child = element->FirstChildElement(name);
	if( child == nullptr ) {
		return defaultValue;
	}
	const char* value = element->FirstChildElement(name)->GetText();	
	return value ? strcmp(value, "true") == 0 : false;
}

} } // LAE::xml

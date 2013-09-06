#pragma once
#ifndef _XML_H_
#define _XML_H_

#include "../exception/Exception.h"
// stl
#include <string>
// xml
#include <tinyxml.h>
// boost
#include <boost/lexical_cast.hpp>
#include <boost/call_traits.hpp>

namespace LAE {
namespace xml {

template<typename T>
void saveAttribute( TiXmlElement* element, const char* name, T value ) {
	TiXmlElement* attrE = new TiXmlElement( name );
	attrE->LinkEndChild( new TiXmlText( boost::lexical_cast<std::string>(value).c_str() ) );
	element->LinkEndChild( attrE );
}

template<>
void saveAttribute<const std::string&>( TiXmlElement* element, const char* name, const std::string& value );

template<>
void saveAttribute<bool>( TiXmlElement* element, const char* name, bool value );

template<>
void saveAttribute<unsigned char>( TiXmlElement* element, const char* name, unsigned char value );

/// load mandatory element
template<typename T>
T loadAttribute( const TiXmlElement* element, const char* name ) {
	const TiXmlElement* child = element->FirstChildElement(name);
	if( child == nullptr ) {
		throw LAEException("Missing element ") << name;
	}
	return boost::lexical_cast<T>( child->GetText() );
}

template<>
std::string loadAttribute<std::string>( const TiXmlElement* element, const char* name );

template<>
bool loadAttribute<bool>( const TiXmlElement* element, const char* name );

/// load optional element
template<typename T>
T loadAttribute( const TiXmlElement* element, const char* name, typename boost::call_traits<T>::param_type defaultValue ) {
	const TiXmlElement* child = element->FirstChildElement(name);
	return child == nullptr ? defaultValue : boost::lexical_cast<T>( child->GetText() );
}

template<>
std::string loadAttribute<std::string>( const TiXmlElement* element, const char* name, const std::string& defaultValue );

template<>
bool loadAttribute<bool>( const TiXmlElement* element, const char* name, bool defaultValue );

} } // LAE::xml

#endif // _XML_H_

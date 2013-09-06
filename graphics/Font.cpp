// LAE
#include "Font.h"
#include "../xml/Xml.h"
#include "../exception/Exception.h"
// stl
#include <fstream>
// xml

namespace LAE {

Font::Font()
	: charWidth_( 0 )
	, charHeight_( 0 )
	, charCols_( 12 )
	, charRows_( 12 ) {
	memset( left_, 0, sizeof(left_) );
	memset( right_, 0, sizeof(right_) );
	memset( charMap_, 0, sizeof(charMap_) );
}

Font::~Font() {
}

void Font::loadBin( const std::string& fname ) {
	std::ifstream is;
	is.open( fname.c_str() );

	if( !is.is_open() ) {
		throw LAEException( "Failed to load font '" ) << fname << "'";
	}

	char buffer[202];
	unsigned char* ubuff = reinterpret_cast<unsigned char*>( buffer );

	is.read( buffer, 202 );

	charWidth_ = *( ubuff++ );
	charHeight_ = *( ubuff++ );
	for( int i = 0; i < 100; i++ ) {
		left_[i] = *( ubuff++ );
		right_[i] = *( ubuff++ );
	}

	is.close();
}

void Font::load( const std::string& fname, const std::string& textFile ) {
	std::ifstream textStream(textFile);
	if( !textStream.is_open() ) {
		throw LAEException("Error loading font text") << textFile;
	}

	char text[256];
	textStream.read(text, 256);
	textStream.clear();
	std::streamoff textSize = textStream.tellg();

	textStream.close();

	// nacteni
	TiXmlDocument document;
	if( !document.LoadFile(fname.c_str()) ) {
		throw LAEException("Error loading font ") << fname;
	}

	TiXmlElement* fontE = document.RootElement();
	charWidth_ = xml::loadAttribute<int>( fontE, "width" );
	charHeight_ = xml::loadAttribute<int>( fontE, "height" );

	memset(left_, 0, sizeof(left_));
	memset(right_, 0, sizeof(right_));
	memset(charMap_, 0, sizeof(charMap_));

	size_t index = 0;
	for( TiXmlElement* charE = fontE->FirstChildElement("char"); charE; charE = charE->NextSiblingElement("char") ) {
		std::string left = charE->Attribute("left");
		std::string right = charE->Attribute("right");

		left_[index] = boost::lexical_cast<int>(left);
		right_[index] = boost::lexical_cast<int>(right);

		unsigned char charI = static_cast<unsigned char>(text[index]);
		charMap_[charI] = index;

		if( ++index >= textSize ) {
			break;
		}
	}
}

int Font::getWidth( const std::string& text ) const {
	int width = 0;
	int ch;
	for( size_t i = 0, n = text.length(); i < n; i++ ) {
		if( text[i] == ' ' ) {
			width += charWidth_ / 2;
			continue;
		}
		ch = charMap_[static_cast<int>(text[i])];
		width += right_[ch] - left_[ch];
	}
	return width;
}

}

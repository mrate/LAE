// LAE
#include "TextStorage.h"
#include "../logger/Logger.h"
// stl
#include <fstream>

namespace LAE {

TextStorage::TextStorage() {
}

TextStorage::~TextStorage() {
}

void TextStorage::load( const std::string& fname ) {
	char line[256];
	string param;
	string text;

	ifstream is;
	is.open( fname.c_str() );
	if( !is.is_open() ) {
		LOG_ERROR( "Failed to open file '" << fname << "'" );
		return ;
	}

	while( !is.eof() ) {
		param = "";
		text = "";

		is.getline( line, 256, '=' );
		param.append( line );
		is.getline( line, 256 );
		text.append( line );

		if( stringMap_.count( param ) != 0 ) {
			//debug_cout("retezec '" << param << "' definovan vicekrat!" << endl);
			break;
		} else {
			//debug_cout("stringMap[" << param << "]=" << text << endl);
			stringMap_[param] = text;
		}
	}

	is.close();
}

const std::string& TextStorage::getText( const std::string& text ) {
	if( stringMap_.count( text ) == 0 ) {
		LOG_ERROR( "Text '" << text << "' not found in string map" );
		return text;
	} else {
		return stringMap_[text];
	}
}

}

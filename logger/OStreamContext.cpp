#include "OStreamContext.h"
//stl
#include <fstream>

namespace logger {

OStreamContext::OStreamContext( std::ostream& output )
	: fileOutput_( nullptr )
	, output_( output )
	, errorOutput_( output ) {
}

OStreamContext::OStreamContext( std::ostream& output, std::ostream& errorOutput ) 
	: fileOutput_( nullptr )
	, output_( output )
	, errorOutput_( errorOutput ) {
}

OStreamContext::OStreamContext( const char* fname, bool append )
	: fileOutput_( new std::fstream( fname, append ? std::ios::app : std::ios::out ) )
	, output_( *fileOutput_ ) 
	, errorOutput_( *fileOutput_ ) {
}

OStreamContext::~OStreamContext() {
}

void OStreamContext::log( Scope scope, const char* msg ) {
	static const char* scopeStr[] = {
		"INFO",
		"DETAIL",
		"DEBUG",
		"ERROR"
	};

	std::ostream* out = scope == SCOPE_ERROR ? &errorOutput_ : &output_;
	(*out) << "[" << scopeStr[scope] << "] " << msg;
}

}

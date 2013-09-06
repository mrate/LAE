#pragma once
#ifndef _OSTREAMCONTEXT_H_
#define _OSTREAMCONTEXT_H_

#include "Logger.h"

namespace logger {

class OStreamContext : public LogContext {
public:
	OStreamContext( std::ostream& output );
	OStreamContext( std::ostream& output, std::ostream& errorOutput );
	OStreamContext( const char* fname, bool append = false );
	virtual ~OStreamContext();

	virtual void log( Scope scope, const char* msg ) override;

private:
	std::ostream* fileOutput_;
	std::ostream& output_;
	std::ostream& errorOutput_;
};

}

#endif // _OSTREAMCONTEXT_H_

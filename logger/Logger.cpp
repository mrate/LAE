#include "Logger.h"
// std
#include <iostream>

namespace logger {
SEndl endl;

Logger::Logger() {
	for( int i = 0; i < SCOPE_COUNT; ++i ) {
		buffer_[ i ].setScope( static_cast<Scope>( i ) );
		buffer_[ i ].setParent( this );
	}
}

Logger::~Logger() {
}

Logger& Logger::instance() {
	static Logger inst;
	return inst;
}

void Logger::flush( Scope scope, const char* msg ) {
	for( ContextQueue::iterator it = ctx_.begin(); it != ctx_.end(); ++it ) {
		( *it )->log( scope, msg );
	}
}

template<>
Logger::LogBuffer& Logger::LogBuffer::operator<< < ::logger::SEndl >( ::logger::SEndl ) {
	buffer_ << "\r\n";
	parent_->flush( scope_, buffer_.str().c_str() );
	buffer_.str( std::string() );
	buffer_.clear();
	return *this;
}

}

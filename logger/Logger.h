#pragma once
#ifndef _LOGGER_H_
#define _LOGGER_H_

// boost
#include <boost/thread.hpp>
// stl
#include <sstream>
#include <deque>

#ifdef WIN32
#	define __FUNC_MACRO__ __FUNCTION__
#else
#	define __FUNC_MACRO__ __PRETTY_FUNCTION__
#endif

// pomocny makra
#ifdef _DEBUG
#define LOG_LEVEL( scope, stream ) { \
	boost::mutex::scoped_lock lck(logger::Logger::instance().mtx); \
	logger::cout( scope ) << "<" << (std::hex) << boost::this_thread::get_id() << "> " << __FUNC_MACRO__  << ": " << stream; \
	}
#else
#define LOG_LEVEL( scope, stream ) { \
		boost::mutex::scoped_lock lck(logger::Logger::instance().mtx); \
		logger::cout( scope ) << (std::hex) << boost::this_thread::get_id() << ": " << stream; \
	}
#endif //_DEBUG

#define LOG_INFO(stream) LOG_LEVEL( logger::SCOPE_INFO, stream << logger::endl )
#define LOG_INFO_DETAIL(stream) LOG_LEVEL( logger::SCOPE_INFO_DETAIL, stream << logger::endl )
#define LOG_DEBUG(stream) LOG_LEVEL( logger::SCOPE_DEBUG, stream << logger::endl )
#define LOG_ERROR(stream) LOG_LEVEL( logger::SCOPE_ERROR, stream << logger::endl )

namespace logger {

struct SEndl {};
extern SEndl endl;

enum Scope {
    SCOPE_INFO = 0,
    SCOPE_INFO_DETAIL = 1,
    SCOPE_DEBUG = 2,
    SCOPE_ERROR = 3,

    SCOPE_COUNT = ( SCOPE_ERROR + 1 )
};

class LogContext {
public:
	virtual ~LogContext() {}
	virtual void log( Scope scope, const char* msg ) = 0;
};

class Logger {
public:
	static Logger& instance();
	boost::mutex mtx;

	class LogBuffer {
	public:
		template< typename T >
		LogBuffer& operator<<( T t ) {
			buffer_ << t;
			return *this;
		}


	private:
		LogBuffer() : scope_( SCOPE_INFO ), parent_( 0 ) {}
		void setScope( Scope scope ) {
			scope_ = scope;
		}
		void setParent( Logger* parent ) {
			parent_ = parent;
		}
		// buffer + rozsah
		std::stringstream	buffer_;
		Scope scope_;
		Logger* parent_;
		// kvuli zapisu
		friend class ::logger::Logger;
	};

	static LogBuffer& log( Scope scope ) {
		return instance().buffer_[scope];
	}

	static void addContext( LogContext* ctx ) {
		instance().addContextInner( ctx );
	}

private:
	typedef std::deque<LogContext*> ContextQueue;

	Logger();
	virtual ~Logger();

	void flush( Scope scope, const char* msg );
	void addContextInner( LogContext* ctx ) {
		ctx_.push_back( ctx );
	}

	LogBuffer		buffer_[4];
	ContextQueue	ctx_;
};

template<>
Logger::LogBuffer& Logger::LogBuffer::operator<< < ::logger::SEndl >( ::logger::SEndl );

inline Logger::LogBuffer& cout( Scope scope ) {
	return Logger::log( scope );
}
}

#endif // _LOGGER_H_

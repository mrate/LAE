#pragma once

#include <string>
#include <sstream>

namespace LAE {
class LAEException : public std::exception {
public:
	LAEException() : std::exception() {
	}

	LAEException( const char* msg ) : std::exception(), str( msg ) {
	}

	LAEException( const std::string& msg ) : std::exception(), str( msg ) {
	}

	virtual ~LAEException() throw() {}

	template<typename T>
	LAEException& operator<<( const T& t ) {
		std::stringstream tm;
		tm << str;
		tm << t;
		str = tm.str();
		return *this;
	}

	template<typename T>
	LAEException& operator<<( const T* t ) {
		std::stringstream tm;
		tm << str;
		tm << t;
		str = tm.str();
		return *this;
	}

	virtual const char* what() const throw() {
		return str.c_str();
	}

private:
	std::string str;
};
}

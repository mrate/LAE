#pragma once
#ifndef _TEXTSTORAGE_H_
#define _TEXTSTORAGE_H_

// LAE
#include "../types/types.h"
// stl
#include <string>

namespace LAE {

class TextStorage {
public:
	typedef StringMap::iterator iterator;
	typedef StringMap::const_iterator const_iterator;

	TextStorage();
	~TextStorage();

	const std::string& getText( const std::string& text );
	void load( const std::string& fname );
	
	void clear() { stringMap_.clear(); }

	// iterator
	iterator find( const std::string& name ) { return stringMap_.find(name); }
	iterator begin() { return stringMap_.begin(); }
	iterator end() { return stringMap_.end(); }
	const_iterator find( const std::string& name ) const { return stringMap_.find(name); }
	const_iterator begin() const { return stringMap_.begin(); }
	const_iterator end() const { return stringMap_.end(); }

private:
	StringMap stringMap_;
};

}

#endif // _TEXTSTORAGE_H_

#pragma once
#ifndef _UTILS_H_
#define _UTILS_H_

// LAE
#include "../logger/Logger.h"
#include "../types/types.h"
// stl
#include <string>
#include <iostream>

namespace LAE {
template<class InputIterator, class Function>
Function for_each_key( InputIterator first, InputIterator last, Function f ) {
	for( ; first != last; ++first ) {
		f( first->first );
	}
	return f;
}

template<class InputIterator, class Function>
Function for_each_value( InputIterator first, InputIterator last, Function f ) {
	for( ; first != last; ++first ) {
		f( first->second );
	}
	return f;
}

//bool lineIntersect(const Point l1[],const Point l2[], int& x, int& y);
//bool pointInBox(const SBox& box, int x, int y);
}

#endif // _UTILS_H_

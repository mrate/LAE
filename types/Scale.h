#pragma once
#ifndef _SCALE_H_
#define _SCALE_H_

#include "../math/MathUtils.h"
// stl
#include <vector>

namespace LAE {

struct Scale : public Segment {
	//SPoint p1;
	//SPoint p2;
	unsigned char scale1;
	unsigned char scale2;

	Scale() : scale1( 0 ), scale2( 0 ) {
	}
};

typedef std::vector<Scale>	Scales;

}

#endif // _SCALE_H_

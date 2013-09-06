#pragma once
#ifndef _BOX_H_
#define _BOX_H_

#include "../math/MathUtils.h"
#include "SharedArrays.h"
// stl
#include <vector>
#include <map>
#include <set>

namespace LAE {

struct WalkingBox : public Polygon {
	short z;	// z-order of box
	unsigned char scale;	// index of scale
	bool active;			// is active

	std::map<int, Segment> inter;	// map of intersections
	std::set<int>	neighbour;		// set of neighbour walking boxes

	WalkingBox() : z( 0 ), scale( 0 ), active( true ) {}

	bool hasNeighbour( int n ) const { return neighbour.find(n) != neighbour.end(); }
};
typedef std::vector<WalkingBox> WalkingBoxes;

}

#endif // _BOX_H_

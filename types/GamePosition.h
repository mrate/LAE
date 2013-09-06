#pragma once
#ifndef _GAMEPOSITION_H_
#define _GAMEPOSITION_H_

#include "../math/MathUtils.h"
// stl
#include <deque>

namespace LAE {

struct GamePosition : public Point {
	int boxIndex;

	GamePosition() : boxIndex(0) {}
	GamePosition( const Point& point ) : Point(point), boxIndex(0) {}
};

typedef std::deque<GamePosition> GamePath;

}

#endif // _GAMEPOSITION_H_

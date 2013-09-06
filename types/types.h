#ifndef _TYPES_H_
#define _TYPES_H_

// LAE
#include "../math/MathUtils.h"
#include "../math/point.h"
#include "Box.h"
#include "GamePosition.h"
#include "InventoryItem.h"
#include "KeyboardState.h"
#include "MouseState.h"
#include "Scale.h"
#include "SharedArrays.h"
#include "Sprite.h"
// stl
#include <string>
#include <vector>
#include <map>

#define sgn(x)	(x < 0 ? -1 : x == 0 ? 0 : 1)

namespace LAE {

enum EDirections {
    DIR_L,
    DIR_R,
    DIR_U,
    DIR_D,
    DIR_LU,
    DIR_LD,
    DIR_RU,
    DIR_RD
};

const int DIRECTIONS = 2;

typedef std::vector<int> IntVector;
typedef std::vector<std::string> StringVector;
typedef std::map<std::string, std::string> StringMap;

template<typename T>
struct Deleter {
	Deleter() {}
	void operator()( T* t ) { delete t; }
};

}

#endif //_TYPES_H_
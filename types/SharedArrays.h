#pragma once
#ifndef _SHAREDARRAYS_H_
#define _SHAREDARRAYS_H_

#include <boost/shared_array.hpp>

#include "../math/MathUtils.h"

namespace LAE {

typedef boost::shared_array<Segment> TSHSegmentArray;
typedef boost::shared_array<bool> TSHBoolArray;
typedef boost::shared_array<int> TSHIntArray;

}

#endif // _SHAREDARRAYS_H_

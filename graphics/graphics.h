#pragma once

#include <vector>

namespace LAE {

/// struktura svetelneho zdroje
struct LightSource {
	short	x;					///< x-ova souradnice
	short	y;					///< y-ova souradnice
	unsigned char	intensity;	///< intenzite
	bool	active;				///< zdroj aktivni

	LightSource() : x( 0 ), y( 0 ), intensity( 255 ), active( true ) {
	}
};

typedef std::vector<LightSource> Lights;

}

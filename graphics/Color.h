#ifndef _COLOR_H_
#define _COLOR_H_

namespace LAE {

/// Simple RGBA color struct
struct Color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	Color() : r(255), g(255), b(255), a(255) {}
	Color( unsigned char r, unsigned char g, unsigned char b ) : r(r), g(g), b(b), a(255) {}
	Color( unsigned char r, unsigned char g, unsigned char b, unsigned char a ) : r(r), g(g), b(b), a(a) {}
};

}

#endif //_COLOR_H_

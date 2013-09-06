#ifndef _MATHUTILS_H_
#define _MATHUTILS_H_

#include "common.h"
#include "point.h"
#include "vector.h"

namespace LAE {

const double SMALL_NUM = 0.00000001f; // anything that avoids division overflow

Vector operator-( const Point& p1, const Point& p2 );      // Vector difference

// Usecka
class Segment {
public:
	Segment( const Point& p0, const Point& p1 ) : p0( p0 ), p1( p1 ) {
	}

	Segment() {
	}

	Point p0;
	Point p1;

	double length() const {
		return std::sqrt( std::pow( p1.x - p0.x, 2 ) * std::pow( p1.y - p0.y, 2 ) );
	}
};

// 4-uhelnik
class Polygon {
public:
	Polygon( const Point& p0, const Point& p1, const Point& p2, const Point& p3 ) 
		: center( ( p0 + p1 + p2 + p3 ) / 4 ) {
		p[0] = p0;
		p[1] = p1;
		p[2] = p2;
		p[3] = p3;
	}

	Polygon() {
	}

	Point p[4];
	Point center;
};

// find intersection point of two lines
bool lineIntersect( const Segment& line1, const Segment& line2, Point& intersectionPoint );
bool lineIntersect( const Point l1[], const Point l2[], int& x, int& y );

bool pointInBox( const Polygon& box, const Point& p );

// find intersection line of two polygons
bool findInter( const Polygon& p1, const Polygon& p2, Segment& sinter );

// vzdalenost dvou segmentu, dp1 = nejblizsi bod 1. segmentu k 2., dp2 = nejblizsi bod 2. segmentu k 1.
double dist3D_Segment_to_Segment( const Segment& S1, const Segment& S2, Point* dp1 = 0, Point* dp2 = 0 );
// zjisti, zda se dva segmenty protinaji (0 = neprotinaji)
int intersect2D_Segments( const Segment& S1, const Segment& S2, Point* I0 = 0, Point* I1 = 0 );
// lezi bod na segmentu
int inSegment( const Point& P, const Segment& S );
// vzdalenost bodu od segmentu, CP = nejblizsi bod segmentu k danymu bodu
double dist_Point_to_Segment( const Point& P, const Segment& S, Point* CP = 0 );

// pokusny fce
int cn_PnPoly( const Point& P, const Point* V, int n );

bool inPolygon( const Point& P, const Polygon& PL );
}

#endif //_MATHUTILS_H_

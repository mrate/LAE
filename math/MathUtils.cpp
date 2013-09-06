#include "MathUtils.h"
#include "point.h"
// stl
#include <math.h>

namespace LAE {

Vector operator-( const Point& p1, const Point& p2 ) {   // Vector diff of Points
	Vector v;
	v.x = p1.x - p2.x;
	v.y = p1.y - p2.y;
	v.z = p1.z - p2.z;
	v.dimn = max( p1.dimn, p2.dim() );
	return v;
}

double dot( const Point& u, const Point& v ) {
	return ( ( u ).x * ( v ).x + ( u ).y * ( v ).y + ( u ).z * ( v ).z );
}
double norm( const Point& v ) {
	return sqrt( dot( v, v ) );    // norm = length of vector
}
//double d(const Point& u, const Point& v) { return norm(u-v); }       // distance = norm of difference
double perp( const Point& u, const Point& v ) {
	return ( ( u ).x * ( v ).y - ( u ).y * ( v ).x );    // perp product (2D)
}

// nalezeni pruseciku dvou polygonu
bool findInter( const Polygon& p1, const Polygon& p2, Segment& sinter ) {
	bool po = true;
	Point po1;
	for( int i = 0; i < 4; ++i )
		for( int j = 0; j < 4; ++j ) {
			if( intersect2D_Segments( Segment( p1.p[i], p1.p[( i + 1 ) % 4] ), Segment( p2.p[j], p2.p[( j + 1 ) % 4] ), &po1 ) ) {
				if( po ) {
					sinter.p0 = po1;
					po = false;
				} else if( d( sinter.p0, po1 ) > 2 ) {
					sinter.p1 = po1;
					return true;
				}
			}
		}

	if( !po ) { // protinaj se v 1 bode
		sinter.p1 = sinter.p0;
	}

	return !po;
}

double dist3D_Segment_to_Segment( const Segment& S1, const Segment& S2, Point* dp1, Point* dp2 ) {
	Vector   u = S1.p1 - S1.p0;
	Vector   v = S2.p1 - S2.p0;
	Vector   w = S1.p0 - S2.p0;
	double   a = dot( u, u );     // always >= 0
	double   b = dot( u, v );
	double   c = dot( v, v );     // always >= 0
	double   d = dot( u, w );
	double   e = dot( v, w );
	double   D = a * c - b * b;   // always >= 0
	double   sc, sN, sD = D;      // sc = sN / sD, default sD = D >= 0
	double   tc, tN, tD = D;      // tc = tN / tD, default tD = D >= 0

	// compute the line parameters of the two closest points
	if( D < SMALL_NUM ) { // the lines are almost parallel
		sN = 0.0;        // force using point P0 on segment S1
		sD = 1.0;        // to prevent possible division by 0.0 later
		tN = e;
		tD = c;
	} else {              // get the closest points on the infinite lines
		sN = ( b * e - c * d );
		tN = ( a * e - b * d );
		if( sN < 0.0 ) {      // sc < 0 => the s=0 edge is visible
			sN = 0.0;
			tN = e;
			tD = c;
		} else if( sN > sD ) { // sc > 1 => the s=1 edge is visible
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if( tN < 0.0 ) {          // tc < 0 => the t=0 edge is visible
		tN = 0.0;
		// recompute sc for this edge
		if( -d < 0.0 ) {
			sN = 0.0;
		} else if( -d > a ) {
			sN = sD;
		} else {
			sN = -d;
			sD = a;
		}
	} else if( tN > tD ) {   // tc > 1 => the t=1 edge is visible
		tN = tD;
		// recompute sc for this edge
		if( ( -d + b ) < 0.0 ) {
			sN = 0;
		} else if( ( -d + b ) > a ) {
			sN = sD;
		} else {
			sN = ( -d + b );
			sD = a;
		}
	}
	// finally do the division to get sc and tc
	sc = ( abs( sN ) < SMALL_NUM ? 0.0 : sN / sD );
	tc = ( abs( tN ) < SMALL_NUM ? 0.0 : tN / tD );

	// get the difference of the two closest points
	Vector   dP = w + ( sc * u ) - ( tc * v ); // = S1(sc) - S2(tc)
	if( dp1 ) {
		*dp1 = S2.p0 + w + ( sc * u );
	}
	if( dp2 ) {
		*dp2 = S2.p0 + ( tc * v );
	}
	return norm( dP ); // return the closest distance
}

int intersect2D_Segments( const Segment& S1, const Segment& S2, Point* I0, Point* I1 ) {
	Vector    u = S1.p1 - S1.p0;
	Vector    v = S2.p1 - S2.p0;
	Vector    w = S1.p0 - S2.p0;
	double    D = perp( u, v );

	// test if they are parallel (includes either being a point)
	if( fabs( D ) < SMALL_NUM ) {       // S1 and S2 are parallel
		if( perp( u, w ) != 0 || perp( v, w ) != 0 ) {
			return 0;                   // they are NOT collinear
		}
		// they are collinear or degenerate
		// check if they are degenerate points
		double du = dot( u, u );
		double dv = dot( v, v );
		if( du == 0 && dv == 0 ) {      // both segments are points
			if( S1.p0 != S2.p0 ) {      // they are distinct points
				return 0;
			}
			if( I0 ) {
				*I0 = S1.p0;    // they are the same point
			}
			return 1;
		}
		if( du == 0 ) {                 // S1 is a single point
			if( inSegment( S1.p0, S2 ) == 0 ) { // but is not in S2
				return 0;
			}
			if( I0 ) {
				*I0 = S1.p0;
			}
			return 1;
		}
		if( dv == 0 ) {                 // S2 a single point
			if( inSegment( S2.p0, S1 ) == 0 ) { // but is not in S1
				return 0;
			}
			if( I0 ) {
				*I0 = S2.p0;
			}
			return 1;
		}
		// they are collinear segments - get overlap (or not)
		double t0, t1;                   // endpoints of S1 in eqn for S2
		Vector w2 = S1.p1 - S2.p0;
		if( v.x != 0 ) {
			t0 = w.x / v.x;
			t1 = w2.x / v.x;
		} else {
			t0 = w.y / v.y;
			t1 = w2.y / v.y;
		}
		if( t0 > t1 ) {                 // must have t0 smaller than t1
			double t = t0;
			t0 = t1;
			t1 = t;  // swap if not
		}
		if( t0 > 1 || t1 < 0 ) {
			return 0;     // NO overlap
		}
		t0 = t0 < 0 ? 0 : t0;           // clip to min 0
		t1 = t1 > 1 ? 1 : t1;           // clip to max 1
		if( t0 == t1 ) {                // intersect is a point
			if( I0 ) {
				*I0 = S2.p0 + t0 * v;
			}
			return 1;
		}

		// they overlap in a valid subsegment
		if( I0 ) {
			*I0 = S2.p0 + t0 * v;
		}
		if( I1 ) {
			*I1 = S2.p0 + t1 * v;
		}
		return 2;
	}

	// the segments are skew and may intersect in a point
	// get the intersect parameter for S1
	double     sI = perp( v, w ) / D;
	if( sI < 0 || sI > 1 ) {            // no intersect with S1
		return 0;
	}

	// get the intersect parameter for S2
	double     tI = perp( u, w ) / D;
	if( tI < 0 || tI > 1 ) {            // no intersect with S2
		return 0;
	}

	if( I0 ) {
		*I0 = S1.p0 + sI * u;    // compute S1 intersect point
	}
	return 1;
}

int inSegment( const Point& P, const Segment& S ) {
	if( S.p0.x != S.p1.x ) {   // S is not vertical
		if( S.p0.x <= P.x && P.x <= S.p1.x ) {
			return 1;
		}
		if( S.p0.x >= P.x && P.x >= S.p1.x ) {
			return 1;
		}
	} else {  // S is vertical, so test y coordinate
		if( S.p0.y <= P.y && P.y <= S.p1.y ) {
			return 1;
		}
		if( S.p0.y >= P.y && P.y >= S.p1.y ) {
			return 1;
		}
	}
	return 0;
}

double dist_Point_to_Segment( const Point& P, const Segment& S, Point* CP ) {
	Vector v = S.p1 - S.p0;

	double c1 = dot( P - S.p0, v );
	if( c1 <= 0 ) {
		if( CP ) {
			*CP = S.p0;
		}
		return d( P, S.p0 );
	}

	double c2 = dot( v, v );
	if( c2 <= c1 ) {
		if( CP ) {
			*CP = S.p1;
		}
		return d( P, S.p1 );
	}

	Point tP = S.p0 + ( c1 / c2 ) * v;
	if( CP ) {
		*CP = tP;
	}
	return d( P, tP );
}

int cn_PnPoly( const Point& P, const Point* V, int n ) {
	double vt;
	int    cn = 0;    // the crossing number counter

	// loop through all edges of the polygon
	for( int i = 0; i < n; i++ ) { // edge from V[i] to V[i+1]
		if( ( ( V[i].y <= P.y ) && ( V[i + 1].y > P.y ) ) // an upward crossing
		        || ( ( V[i].y > P.y ) && ( V[i + 1].y <= P.y ) ) ) { // a downward crossing
			// compute the actual edge-ray intersect x-coordinate
			vt = ( P.y - V[i].y ) / ( V[i + 1].y - V[i].y );
			if( P.x < V[i].x + vt * ( V[i + 1].x - V[i].x ) ) { // P.x < intersect
				++cn;    // a valid crossing of y=P.y right of P.x
			}
		}
	}
	return ( cn & 1 ); // 0 if even (out), and 1 if odd (in)
}

bool inPolygon( const Point& P, const Polygon& PL ) {
	Point center( 0, 0 );
	for( int i = 0; i < 4; ++i ) {
		center = center + PL.p[i];
	}
	center = center / 4;

	Segment s1( center, P );
	for( int i = 0; i < 4; ++i ) {
		if( intersect2D_Segments( s1, Segment( PL.p[i], PL.p[( i + 1 ) % 4] ) ) ) {
			return false;
		}
	}
	return true;

	//return cn_PnPoly(P, PL.p, 4);
}

bool lineIntersect( const Segment& line1, const Segment& line2, Point& intersectionPoint ) {
	int x;
	int y;

	Point p1[2] = { line1.p0, line1.p1 };
	Point p2[2] = { line2.p0, line2.p1 };

	if( lineIntersect( p1, p2, x, y ) ) {
		intersectionPoint = Point(x, y);
		return true;
	} else {
		return false;
	}
}

bool lineIntersect( const Point l1[], const Point l2[], int& ix, int& iy ) {
	double A1 = l1[1].y - l1[0].y;
	double B1 = l1[0].x - l1[1].x;
	double C1 = A1 * l1[0].x + B1 * l1[0].y;

	double A2 = l2[1].y - l2[0].y;
	double B2 = l2[0].x - l2[1].x;
	double C2 = A2 * l2[0].x + B2 * l2[0].y;

	double det = A1 * B2 - A2 * B1;

	if( det == 0 ) {
		return false;
	}

	double x = ( B2 * C1 - B1 * C2 ) / det;
	double y = ( A1 * C2 - A2 * C1 ) / det;

	ix = x;
	iy = y;

	return (
	           std::min( l1[0].x, l1[1].x ) <= x
	           && x <= std::max( l1[0].x, l1[1].x )
	           && std::min( l1[0].y, l1[1].y ) <= y
	           && y <= std::max( l1[0].y, l1[1].y )

	           && std::min( l2[0].x, l2[1].x ) <= x
	           && x <= std::max( l2[0].x, l2[1].x )
	           && std::min( l2[0].y, l2[1].y ) <= y
	           && y <= std::max( l2[0].y, l2[1].y )
	       );
}

bool pointInBox( const Polygon& box, const Point& p ) {
	int ix, iy;
	Point l1[2], l2[2];

	double minX = box.p[0].x;
	double minY = box.p[0].y;
	double maxX = box.p[0].x;
	double maxY = box.p[0].y;
	for( int i = 1; i < 4; ++i ) {
		minX = std::min( minX, box.p[i].x );
		minY = std::min( minY, box.p[i].y );
		maxX = std::max( maxX, box.p[i].x );
		maxY = std::max( maxY, box.p[i].y );
	}

	if( p.x < minX || p.x > maxX || p.y < minY || p.y > maxY ) {
		return false;
	}

	l1[0] = p;
	l1[1] = box.center;
	for( int i = 0; i < 4; ++i ) {
		l2[0] = box.p[i];
		l2[1] = box.p[( i + 1 ) % 4];

		if( lineIntersect( l1, l2, ix, iy ) ) {
			return false;
		}
	}
	return true;
}

}

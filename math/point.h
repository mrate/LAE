//==================================================================
// Copyright 2002, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from it's use.
// Users of this code must verify correctness for their application.
//==================================================================
#pragma once

#include "common.h"

#include <iosfwd>

using namespace std;

namespace LAE {

class Vector;

//==================================================================
//  Point Class Definition
//==================================================================

class Point {
	friend class Vector;
protected:
	int dimn;            // # coords (1, 2, or 3 max here)
	Error err;           // error indicator
public:
	double x, y, z;      // z=0 for 2D, y=z=0 for 1D

	//----------------------------------------------------------
	// Lots of Constructors (add more as needed)
	Point() {
		dimn = 2;
		x = y = z = 0;
		err = Enot;
	}
	// 1D Point
	Point( int a ) {
		dimn = 1;
		x = a;
		y = z = 0;
		err = Enot;
	}
	Point( double a ) {
		dimn = 1;
		x = a;
		y = z = 0;
		err = Enot;
	}
	// 2D Point
	Point( int a, int b ) {
		dimn = 2;
		x = a;
		y = b;
		z = 0;
		err = Enot;
	}
	Point( double a, double b ) {
		dimn = 2;
		x = a;
		y = b;
		z = 0;
		err = Enot;
	}
	// 3D Point
	Point( int a, int b, int c ) {
		dimn = 3;
		x = a;
		y = b;
		z = c;
		err = Enot;
	}
	Point( double a, double b, double c ) {
		dimn = 3;
		x = a;
		y = b;
		z = c;
		err = Enot;
	}
	// n-dim Point
	Point( int n, int a[] );
	Point( int n, double a[] );
	// Destructor
	~Point() {};

	//----------------------------------------------------------
	// Input/Output streams
	friend istream& operator>>( istream&, Point& );
	friend ostream& operator<<( ostream&, Point );

	//----------------------------------------------------------
	// Assignment "=": use the default to copy all members
	int dim() const {
		return dimn;    // get dimension
	}
	int setdim( int );              // set new dimension

	//----------------------------------------------------------
	// Comparison (dimension must match, or not)
	int operator==( const Point& ) const;
	int operator!=( const Point& ) const;

	//----------------------------------------------------------
	// Point and Vector Operations (always valid)
	friend Vector operator-( const Point&, const Point& );      // Vector difference

	Point  operator+( const Vector& ) const;     // +translate
	Point  operator-( const Vector& ) const;     // -translate
	Point& operator+=( const Vector& );    // inc translate
	Point& operator-=( const Vector& );    // dec translate

	//----------------------------------------------------------
	// Point Scalar Operations (convenient but often illegal)
	// using any type of scalar (int, float, or double)
	//    are not valid for points in general,
	//    unless they are 'affine' as coeffs of
	//    a sum in which all the coeffs add to 1,
	//    such as: the sum (a*P + b*Q) with (a+b == 1).
	//    The programmer must enforce this (if they want to).

	// Scalar Multiplication
	friend Point operator*( int, const Point& );
	friend Point operator*( double, const Point& );
	friend Point operator*( const Point&, int );
	friend Point operator*( const Point&, double );
	// Scalar Division
	friend Point operator/( const Point&, int );
	friend Point operator/( const Point&, double );

	//----------------------------------------------------------
	// Point Addition (also convenient but often illegal)
	//    is not valid unless part of an affine sum.
	//    The programmer must enforce this (if they want to).
	friend Point operator+( const Point&, const Point& );    // add points

	// Affine Sum
	// Returns weighted sum, even when not affine, but...
	// Tests if coeffs add to 1.  If not, sets: err = Esum.
	friend Point asum( int, int[], const Point[] );
	friend Point asum( int, double[], const Point[] );

	//----------------------------------------------------------
	// Point Relations
	friend double d( const Point&, const Point& );        // Distance
	friend double d2( const Point&, const Point& );       // Distance^2
	double isLeft( const Point&, const Point& );          // 2D only
	double Area( const Point&, const Point& ); 		// any dim for triangle PPP

	// Collinearity Conditions (any dim n)
	bool isOnLine( const Point&, const Point&, char ); // is On line (char= flag)
	bool isOnLine( const Point&, const Point& );       // is On line (flag= all)
	bool isBefore( const Point&, const Point& );       // is On line (flag= before)
	bool isBetween( const Point&, const Point& );      // is On line (flag= between)
	bool isAfter( const Point&, const Point& );        // is On line (flag= after)
	bool isOnRay( const Point&, const Point& );        // is On line (flag= between|after)

	friend std::ostream& operator<<( std::ostream& str, const Point& p );

	//----------------------------------------------------------
	// Error Handling
	void  clerr() {
		err = Enot;   // clear error
	}
	int   geterr() {
		return err;   // get error
	}
	const char* errstr() const;                         // return error string
};
}

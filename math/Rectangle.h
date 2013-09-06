#pragma once
#ifndef _RECTANGLE_H_
#define _RECTANGLE_H_

namespace LAE {

class Rectangle {
public:
	Rectangle() : x_(0), y_(0), width_(0), height_(0) {}
	Rectangle( int x, int y, int width, int height ) 
		: x_( x )
		, y_( y )
		, width_( width )
		, height_( height ) {}

	int x() const { return x_; }
	void setX(int x) { x_ = x; }
	int y() const { return y_; }
	void setY(int y) { y_ = y; }
	int width() const { return width_; }
	void setWidth(int width) { width_ = width; }
	int height() const { return height_; }
	void setHeight(int height) { height_ = height; }

	int left() const { return x_; }
	int right() const { return x_ + width_; }
	int top() const { return y_; }
	int bottom() const { return y_ + height_; }

private:
	int x_;
	int y_;
	int width_;
	int height_;
};

}

#endif // _RECTANGLE_H_

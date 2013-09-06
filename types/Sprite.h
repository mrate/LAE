#pragma once
#ifndef _SPRITE_H_
#define _SPRITE_H_

//stl
#include <string>
#include <vector>
// LAE
#include "../graphics/PImage.h"

namespace LAE {

class Sprite {
public:
	Sprite() : width_( 0 ), height_( 0 ), angle_( 0 ), alpha_( 255 ) {}

	const Point& getPosition() const { return position_; }

	double getX() const { return position_.x; }
	void setX( double x ) { position_.x = x; }
	double getY() const { return position_.y; }
	void setY( double y ) { position_.y = y; }

	PImage getImage() const { return image_; }
	void setImage( PImage image ) { image_ = image; }

private:
	Point position_;
	short width_;
	short height_;
	short angle_;
	unsigned char alpha_;
	PImage image_;
};

typedef std::vector<Sprite> Sprites;

}

#endif // _SPRITE_H_

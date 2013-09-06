#pragma once
#ifndef _CAMERA_H_
#define _CAMERA_H_

// LAE
#include "../types/types.h"
#include "Actor.h"
#include "Item.h"

namespace LAE {
class Camera {
public:
	enum ECameraType {
	    C_FOLLOW_ACTOR,
	    C_FOLLOW_SPRITE,
	    C_MOVE_TO,
	    C_IDLE
	};

	Camera();
	virtual ~Camera();

	void update( unsigned long time );

	void follow( Actor* act );
	void follow( Sprite* spr );
	void focusOnItem( Item* item );
	void setPos( int x, int y );
	void moveTo( int x, int y );
	void setBorders( int w, int h );
	void fastForward();

	bool isIdle() const;

	int getX() const;
	int getY() const;

private:
	void checkBorders( double& px, double& py );

	double x_;
	double y_;
	int width_;
	int height;

	int tx_;
	int ty_;

	int speed_;
	ECameraType followType_;
	Sprite*	sprite_;
	Actor* actor_;
};
}

#endif // _CAMERA_H_

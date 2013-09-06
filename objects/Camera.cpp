// LAE
#include "Camera.h"
#include "../logger/Logger.h"
#include "../game/Global.h"
// stl
#include <math.h>

namespace LAE {

Camera::Camera()
	: x_( 0 )
	, y_( 0 )
	, width_( 0 )
	, height( 0 )
	, tx_( 0 )
	, ty_( 0 )
	, speed_( 5 )
	, followType_( C_IDLE )
	, sprite_( nullptr )
	, actor_( nullptr ) {
}

Camera::~Camera() {
}

void Camera::fastForward() {
	if( followType_ != C_MOVE_TO ) {
		return;
	}
	followType_ = C_IDLE;
	x_ = tx_ - Global::i()->screenWidth() / 2;
	y_ = ty_ - Global::i()->screenHeight() / 2;
}

void Camera::update( unsigned long time ) {
	double dx;
	double dy;
	double lx = x_;
	double ly = y_;

	switch( followType_ ) {
	case C_IDLE:
		dx = x_;
		dy = y_;
		break;
	case C_MOVE_TO:		
		dx = tx_ - Global::i()->screenWidth() / 2;
		dy = ty_ - Global::i()->screenHeight() / 2;
		break;
	case C_FOLLOW_ACTOR:
		dx = actor_->getX() - Global::i()->screenWidth() / 2;
		dy = actor_->getY() - Global::i()->screenHeight() / 2;
		break;
	case C_FOLLOW_SPRITE:
		dx = sprite_->getX() - Global::i()->screenWidth() / 2;
		dy = sprite_->getY() - Global::i()->screenHeight() / 2;
		break;
	}

	checkBorders( dx, dy );
	double angle = ( double )atan2( ( double )( dx - x_ ), ( double )( dy - y_ ) );
	double delta = ( ( double )time / ( double )speed_ );
	x_ += sin( angle ) * delta;
	y_ += cos( angle ) * delta;

	if( ( sgn( lx - dx ) != sgn( x_ - dx ) )
	        || ( sgn( ly - dy ) != sgn( y_ - dy ) ) ) {
		x_ = dx;
		y_ = dy;
		if( followType_ == C_MOVE_TO ) {
			followType_ = C_IDLE;
		}
	}
}

bool Camera::isIdle() const {
	return followType_ == C_IDLE;
}

void Camera::follow( Actor* act ) {
	followType_ = C_FOLLOW_ACTOR;
	actor_ = act;
}

void Camera::follow( Sprite* spr ) {
	followType_ = C_FOLLOW_SPRITE;
	sprite_ = spr;
}

void Camera::setPos( int x, int y ) {
	followType_ = C_IDLE;
	this->x_ = x;
	this->y_ = y;
}

void Camera::moveTo( int x, int y ) {
	LOG_DEBUG( "camera_moveto " << x << ", " << y );
	followType_ = C_MOVE_TO;
	tx_ = x;
	ty_ = y;
}

void Camera::focusOnItem( Item* item ) {
	moveTo( item->getX(), item->getY() );
}

void Camera::setBorders( int w, int h ) {
	width_ = w;
	height = h;
	checkBorders( x_, y_ );
}

void Camera::checkBorders( double& px, double& py ) {
	if( px + Global::i()->screenWidth() > width_ ) {
		px = width_ - Global::i()->screenWidth();
	}
	if( py + Global::i()->screenHeight() > height ) {
		py = height - Global::i()->screenHeight();
	}
	if( px < 0 ) {
		px = 0;
	}
	if( py < 0 ) {
		py = 0;
	}
}

int Camera::getX() const {
	return ( int )x_;
}

int Camera::getY() const {
	return ( int )y_;
}

}

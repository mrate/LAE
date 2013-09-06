#pragma once
#ifndef _ACTOR_H_
#define _ACTOR_H_

// LAE
#include "../types/types.h"
#include "../graphics/Screen.h"
#include "../graphics/AnimationPlayer.h"
#include "../graphics/PAnimation.h"

class TiXmlElement;

namespace LAE {

class Actor {
public:
	enum EActorState {
		STATE_IDLE,
		STATE_GO,
		STATE_ANIMATION,
		STATE_TALK
	};

	Actor();
	virtual ~Actor();

	void load( const std::string& fileName );

	// sirka / vyska
	unsigned short getWidth() const;
	void setWidth( unsigned short width ) { width_ = width; }
	unsigned short getHeight() const;
	void setHeight( unsigned short height ) { height_ = height; }

	// planovani trasy
	void clearPath();
	void addToPath( int x, int y );
	void addToPath( const Point& point );
	void addToPath( const GamePosition& position );
	void addInFront( int x, int y );
	void addInFront( const Point& point );
	const GamePath& getPath() const { return path_; }

	// vykreslovani
	void update( unsigned long time );
	void render( Screen& scr, int x, int y ) const;
	const AnimationPlayer& getAnimationPlayer() const { return animPlayer_; }

	// animace
	void setAnim( PAnimation animation, bool loop );
	void setAnim( PAnimation animation );
	void setAnim( const std::string& fileName, bool loop );
	void setAnim( const std::string& fileName );
	// 
	void setWalkAnim( int dir, PAnimation anim );
	void setWalkAnim( int dir, const std::string& fileName );
	void setIdleAnim( int dir, PAnimation anim );
	void setIdleAnim( int dir, const std::string& fileName );

	// pozice
	const Point& getPosition() const { return position_; }
	void setPosition( const Point& position ) { position_ = position; }
	void setPosition( double x, double y ) { position_.x = x; position_.y = y; }

	double getX() const { return position_.x; }
	void setX( double x ) { position_.x = x; }
	double getY() const { return position_.y; }
	void setY( double y ) { position_.y = y; }
	double getZ() const { return position_.z; }
	void setZ( double z ) { position_.z = z; }

	// index boxu, na kterem stoji
	unsigned char getBoxIndex() const { return boxIndex_; }
	void setBoxIndex( unsigned char boxIndex ) { boxIndex_ = boxIndex; }
	// zmena velikosti
	void setScale( unsigned char scale ) { scale_ = scale; }

	// stav
	void setState( EActorState state );
	EActorState getState() const { return state_; }

private:
	// menici se vlastnosti
	Point position_;
	unsigned char scale_;
	unsigned char boxIndex_;
	unsigned char dir_;
	int speed_;
	AnimationPlayer animPlayer_;
	EActorState state_;

	GamePath path_;
	int dstX_;
	int dstY_;
	PAnimation currentAnim_;

	// konstanty
	unsigned short width_;
	unsigned short height_;

	PAnimation walkAnim_[DIRECTIONS];
	PAnimation idleAnim_[DIRECTIONS];
	PAnimation talkAnim_[DIRECTIONS];

	//
	void readDirections( TiXmlElement* xmlE, PAnimation* targetArray ) const;
	int directionFromString( const char* str ) const;
};

}	// LAE

#endif // _ACTOR_H_

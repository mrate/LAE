// LAE
#include "Actor.h"
#include "../logger/Logger.h"
#include "../cache/AnimationCache.h"
#include "../xml/Xml.h"
// stl
#include <math.h>
#include <cassert>
// xml
#include <tinyxml.h>

namespace LAE {

Actor::Actor()
	: scale_( 255 )
	, boxIndex_( 0 )
	, dir_( DIR_R )
	, speed_( 8 )
	, state_( STATE_IDLE )
	, dstX_( 0 )
	, dstY_( 0 ) 
	, width_( 0 )
	, height_( 0 ) {
}

Actor::~Actor() {
}

void Actor::load( const std::string& fileName ) {
	LOG_DEBUG( "Loading actor '" << fileName << "' ..." );

	try {
		TiXmlDocument document;
		document.LoadFile(fileName.c_str());

		TiXmlElement* roomElement = document.RootElement();
		width_ = xml::loadAttribute<unsigned short>( roomElement, "width" );
		height_ = xml::loadAttribute<unsigned short>( roomElement, "height" );

		LOG_DEBUG( "...width = " << width_ << ", height = " << height_ );

		// walk
		if( TiXmlElement* walk = roomElement->FirstChildElement("walk") ) {
			LOG_DEBUG( "...reading walk animations" );
			readDirections( walk, walkAnim_ );
		}

		// idle
		if( TiXmlElement* idle = roomElement->FirstChildElement("idle") ) {
			LOG_DEBUG( "...reading idle animations" );
			readDirections( idle, idleAnim_ );
		}

		// talk
		if( TiXmlElement* talk = roomElement->FirstChildElement("talk") ) {
			LOG_DEBUG( "...reading talk animations" );
			readDirections( talk, talkAnim_ );
		}
	} catch(...) {
		throw;
	}
	LOG_DEBUG( "Actor '" << fileName << "' loaded" );
}

void Actor::readDirections( TiXmlElement* xmlE, PAnimation* targetArray ) const {
	for( TiXmlElement* dirE = xmlE->FirstChildElement(); dirE; dirE = dirE->NextSiblingElement() ) {
		int direction = directionFromString( dirE->Value() );
		targetArray[direction] = AnimationManager::i()->createObject( dirE->GetText() );
	}
}

int Actor::directionFromString( const char* str ) const {
	if( strcmp( str, "left" ) == 0 ) {
		return DIR_L;
	} else if( strcmp( str, "right" ) == 0 ) {
		return DIR_R;
	} else {
		throw LAEException( "Invalid direction: " ) << str;
	}
}

void Actor::clearPath() {
	path_.erase( path_.begin(), path_.end() );
}

void Actor::addInFront( int x, int y ) {
	GamePosition pos;
	pos.x = x;
	pos.y = y;
	path_.push_front( pos );
}

void Actor::addInFront( const Point& point ) {
	path_.push_front( GamePosition(point) );
}

void Actor::addToPath( int x, int y ) {
	GamePosition pos;
	pos.x = x;
	pos.y = y;
	path_.push_back( pos );
}

void Actor::addToPath( const GamePosition& position ) {
	path_.push_back( GamePosition( position ) );
}

void Actor::addToPath( const Point& point ) {
	path_.push_back( GamePosition( point ) );
}

void Actor::setState( EActorState state ) {
	switch( state ) {
	case STATE_IDLE:
		currentAnim_ = idleAnim_[dir_];
		animPlayer_.start( currentAnim_, true );
		break;
	case STATE_GO:
		if( path_.empty() ) {
			setState( STATE_IDLE );
		} else {
			if( path_.front().x < position_.x ) {
				if( currentAnim_ != walkAnim_[DIR_L] || state_ != STATE_GO ) {
					currentAnim_ = walkAnim_[DIR_L];
					animPlayer_.start( walkAnim_[DIR_L], true );
				}
			} else {
				if( currentAnim_ != walkAnim_[DIR_R] || state_ != STATE_GO ) {
					currentAnim_ = walkAnim_[DIR_R];
					animPlayer_.start( walkAnim_[DIR_R], true );
				}
			}
		}
		break;
	case STATE_TALK:
		currentAnim_ = talkAnim_[dir_];
		animPlayer_.start( currentAnim_, true );
		break;
	default:
		break;
	}
	state_ = state;
	assert( currentAnim_ );
}

void Actor::update( unsigned long time ) {
	switch( state_ ) {
	case STATE_GO: {
		double lx = position_.x;
		double ly = position_.y;
		double dx = path_.front().x;
		double dy = path_.front().y;
		double angle = ( double )atan2( ( double )( dx - position_.x ), ( double )( dy - position_.y ) );
		double delta = ( ( double )scale_ / 255.0f ) * ( ( double )time / ( double )speed_ );

		position_.x += sin( angle ) * delta;
		position_.y += cos( angle ) * delta;

		if( sgn( position_.x - dx ) != sgn( lx - dx ) || sgn( position_.y - dy ) != sgn( ly - dy ) ) {
			position_.x = dx;
			position_.y = dy;
			path_.pop_front();
			if( path_.empty() ) {
				setState( STATE_IDLE );
			} else {
				setState( STATE_GO );
			}
		} else {
			if( angle < 0 ) {
				dir_ = DIR_L;
			} else {
				dir_ = DIR_R;
			}
		}
	}
	break;
	case STATE_IDLE:
	case STATE_ANIMATION:
	case STATE_TALK:
		break;
	}
	animPlayer_.update( time );
}

void Actor::render( Screen& scr, int x, int y ) const {
	scr.setEffect( Screen::EFFECT_NONE );
	animPlayer_.renderScaled( scr, ( int )( x + position_.x ), ( int )( y + position_.y ), scale_ );

	/*
	float lx = x + this->x, ly = y + this->y;
	for(TPathDeque::const_iterator i=path_.begin();i!=path_.end();i++)
	{
		const SGamePosition &pos = *i;
		scr.drawLine(lx, ly, x + pos.x, y + pos.y, 255, 255, 255);
		lx = pos.x + x;
		ly = pos.y + y;
	}
	*/
}

unsigned short Actor::getWidth() const {
	return ( unsigned short )( ( float )width_ * ( float )scale_ / 255.0f );
}

unsigned short Actor::getHeight() const {
	return ( unsigned short )( ( float )height_ * ( float )scale_ / 255.0f );
}

void Actor::setAnim( PAnimation animation, bool loop ) {
	assert( animation.get() );
	LOG_DEBUG( "setting animation: " << animation->getFileName() << ", loop: " << loop );
	animPlayer_.start( animation, loop );
}

void Actor::setAnim( PAnimation animation ) {
	assert( animation.get() );
	setAnim(animation, true);
}

void Actor::setAnim( const std::string& fileName ) {
	setAnim( AnimationManager::i()->createObject(fileName), true);
}

void Actor::setAnim( const std::string& fileName, bool loop ) {
	setAnim( AnimationManager::i()->createObject(fileName), loop);
}

void Actor::setWalkAnim( int dir, PAnimation anim ) {
	LOG_DEBUG( "setting walk animation: " << dir << " - " << anim->getFileName() );
	walkAnim_[dir] = anim;
}

void Actor::setWalkAnim( int dir, const std::string& fileName ) {
	LOG_DEBUG( "setting walk animation: " << dir << " - " << fileName );
	walkAnim_[dir] = AnimationManager::i()->createObject(fileName);
}

void Actor::setIdleAnim( int dir, PAnimation anim ) {
	LOG_DEBUG( "setting idle animation: " << dir << " - " << anim->getFileName() );
	idleAnim_[dir] = anim;
}

void Actor::setIdleAnim( int dir, const std::string& fileName ) {
	LOG_DEBUG( "setting idle animation: " << dir << " - " << fileName );
	idleAnim_[dir] = AnimationManager::i()->createObject(fileName);
}

}

#include "AnimatedSprite.h"
#include "../cache/AnimationCache.h"

namespace LAE {

AnimatedSprite::AnimatedSprite() 
	: x_( 0 )
	, y_( 0 )
	, z_( 0 )
	, effect_( Screen::EFFECT_NONE )
	, lightable_( false )
	, visible_( true ) {
}

AnimatedSprite::~AnimatedSprite() {
}

void AnimatedSprite::setAnimation( PAnimation animation, bool loop ) {
	anim_.start( animation, loop );
}

void AnimatedSprite::update( unsigned long time ) {
	anim_.update( time );
}

void AnimatedSprite::render( Screen& scr, int dx, int dy, unsigned char alpha ) const {
	if( visible_ ) {
		anim_.render( scr, x_ + dx, y_ + dy, alpha );
	}
}

void AnimatedSprite::setAnimation( PAnimation animation ) {
	setAnimation( animation, true );
}

void AnimatedSprite::setAnimation( const std::string& animation ) {
	setAnimation( AnimationManager::i()->createObject(animation), true );
}

void AnimatedSprite::setAnimation( const std::string& animation, bool loop ) {
	setAnimation( AnimationManager::i()->createObject(animation), loop );
}

}

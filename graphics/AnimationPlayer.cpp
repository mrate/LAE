// LAE
#include "AnimationPlayer.h"
#include "Animation.h"
#include "Screen.h"
#include "../audio/MediaPlayer.h"
#include "../cache/SoundCache.h"
#include "../game/GameTimer.h"
#include "../game/Global.h"
#include "../logger/Logger.h"
// stl
#include <cassert>

namespace LAE {

AnimationPlayer::AnimationPlayer()
	: animTime_( 0 )
	, frame_( 0 )
	, loop_( false )
	, playing_( false )
	//, startTime_ ( 0 )
{}

AnimationPlayer::~AnimationPlayer() {
}

void AnimationPlayer::start( PAnimation anim, bool loop ) {
	assert( anim.get() );
	animation_ = anim;
	animTime_ = 0;
	loop_ = loop;
	frame_ = 0;
	playing_ = true;
	//startTime_ = GameTimer::i()->getTime();
}

void AnimationPlayer::stop() {
	playing_ = false;
}

bool AnimationPlayer::isPlaying() const {
	return playing_;
}

void AnimationPlayer::update( unsigned long elapsedTime ) {
	if( playing_ ) {
		animTime_ += elapsedTime;
		if( animTime_ > animation_->getLength() ) {
			if( loop_ ) {
				animTime_ -= animation_->getLength();
			} else {
				playing_ = false;
			}
		}

		int lframe = frame_;
		frame_ = animation_->getFrameIndex( animTime_, loop_ );
		if( frame_ != lframe && animation_->hasFrameSample( frame_ ) ) {
			MediaPlayer::i()->playSound( animation_->getFrameSample( frame_ ), Global::i()->getSoundVolume() );
		}
	}
}

void AnimationPlayer::render( Screen& scr, int x, int y, unsigned char alpha ) const {
	if( animation_ ) {
		//unsigned long animTime = GameTimer::i()->getTime() - startTime_;
		animation_->renderFrame( scr, x, y, animTime_, loop_, alpha );
	}
}

void AnimationPlayer::renderScaled( Screen& scr, int x, int y, unsigned char scale, unsigned char alpha ) const {
	if( animation_ ) {
		//unsigned long animTime = GameTimer::i()->getTime() - startTime_;
		animation_->renderFrameScaled( scr, x, y, animTime_, loop_, scale, alpha );
	}
}

}

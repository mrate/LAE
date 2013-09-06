#pragma once
#ifndef _ANIMATIONPLAYER_H_
#define _ANIMATIONPLAYER_H_

#include "PAnimation.h"

namespace LAE {

class Screen;
	
class AnimationPlayer {
public:
	AnimationPlayer();
	virtual ~AnimationPlayer();

	void start( PAnimation animation, bool loop );
	void stop();
	void update( unsigned long time );

	bool isPlaying() const;
	void render( Screen& scr, int x, int y, unsigned char alpha = 255 ) const;
	void renderScaled( Screen& scr, int x, int y, unsigned char scale, unsigned char alpha = 255 ) const;
	PAnimation getAnimation() const { return animation_; }

private:
	PAnimation animation_;
	unsigned long animTime_;
	unsigned char frame_;
	bool loop_;
	bool playing_;
	//unsigned long startTime_;
};
}

#endif // _ANIMATIONPLAYER_H_

#pragma once

#include <fmod.hpp>

namespace LAE {

class SoundChannel {
public:
	SoundChannel() : channel_( 0 ) {}

private:
	SoundChannel( FMOD::Channel* channel ) : channel_( channel ) {}

	FMOD::Channel* channel_;

	friend class MediaPlayer;
};

}

#pragma once

// fmod
#include <fmod.hpp>
// stl
#include <string>
#include <list>
// LAE
#include "Sound.h"
#include "SoundChannel.h"

#define F_CALLBACKAPI

namespace LAE {

/// Wrapper knihovny FMOD, ktery dovoluje prehravat hudbu z LAE packagu a zvuky typu LAE::Sound
class MediaPlayer {
public:
	void openStream( const std::string& fname );
	void playStream();
	void stopStream();
	void pauseStream( bool pause );
	void setStreamVolume( float volume );

	// play sound sample
	SoundChannel playSound( const Sound* sound, float volume );
	// play sound from file
	SoundChannel playSound( const std::string& fileName, float volume );

	SoundChannel playSound( int channelIndex, const Sound* sound, float volume );
	bool channelPlaying( const SoundChannel& channel );
	void stopChannel( SoundChannel& channel );
	void setChannelVolume( SoundChannel& channel, float volume );

	void update( unsigned long time );

	void loadSound( Sound* sound, const std::string& fname );

	static MediaPlayer* i();

private:
	struct PlayingSound {
		PSound sound;
		SoundChannel channel;
	};

	MediaPlayer();
	virtual ~MediaPlayer();

	void* F_CALLBACKAPI fmod_open( const char* name );
	void F_CALLBACKAPI fmod_close( void* handle );
	int F_CALLBACKAPI fmod_tell( void* handle );
	int F_CALLBACKAPI fmod_seek( void* handle, int pos, signed char mode );
	int F_CALLBACKAPI fmod_read( void* buffer, int size, void* handle );

	bool init_();
	void free_();

	float streamVolume_;
	int activeStream_;
	int streams_;
	FMOD::System* fmodSystem_;
	FMOD::Sound* stream_[2];
	FMOD::Channel* streamChannel_[2];
	bool playing_[2];

	// crossfade
	int fade_;
	unsigned long cTime_;

	std::list<PlayingSound> sounds_;
};
}

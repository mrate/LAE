#include "Sound.h"
#include "MediaPlayer.h"

namespace LAE {

Sound::Sound() 
	: sample_( nullptr ) {
}

Sound::~Sound() {
	free();
}

void Sound::load( const std::string& fname ) {
	free();
	MediaPlayer::i()->loadSound( this, fname );
	sample_->setLoopCount( 0 );
	fileName_ = fname;
	//sample = MediaPlayer::i()->loadSound( FSOUND_UNMANAGED, fname.c_str(), FSOUND_NORMAL, 0, 0 );
	//FSOUND_Sample_SetMode( sample, FSOUND_LOOP_OFF );
}

void Sound::free() {
	if( sample_ ) {
		sample_->release();
	}
	sample_ = nullptr;
	fileName_ = "";
}

}

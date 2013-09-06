//#ifdef WIN32
//#	pragma comment(lib, "../../../libs/fmodapi375win/api/lib/fmodvc.lib")
//#endif
//
#include "MediaPlayer.h"
#include "../game/Global.h"
#include "../game/GameEngine.h"
#include "../cache/SoundCache.h"
#include "../../LAE_FileSystem/ZReader.h"
#include "../../LAE_FileSystem/FilePackage.h"
// stl
#include <string>

namespace LAE {

namespace {
	const int M_CROSSFADE_TIME = 7500;
	const int M_FADE_TIME = 2500;
	const int M_CROSS_FADE = 1;
	const int M_FADE_IN = 2;
	const int M_FADE_OUT = 3;
}

/// Buffer nactenych dat, umoznuje delat seek a read a vsechno taha z vnitrniho bufferu
struct SBuffer {
	char* buffer;
	size_t size;
	size_t pos;

	SBuffer( size_t size ) : size( size ), pos( 0 ) {
		buffer = new char[size];
	}

	~SBuffer() {
		delete[] buffer;
	}

	void seek( int p, int mode ) {
		switch( mode ) {
		case SEEK_CUR:
			pos += p;
			break;
		case SEEK_SET:
			pos = p;
			break;
		case SEEK_END:
			pos = size + p;
			break;
		}

		if( pos > size ) {
			pos = size;
		}
		//if( pos < 0 ) {
		//	pos = 0;
		//}
	}

	long read( char* data, size_t s ) {
		long toCpy = s;
		if( pos + s > size ) {
			toCpy = size - pos;
		}
		memcpy( data, buffer + pos, toCpy );
		pos += toCpy;
		return toCpy;
	}
};

MediaPlayer::MediaPlayer()
	: streamVolume_( 0 )
	, activeStream_( 0 )
	, streams_( 0 )
	, fmodSystem_( nullptr )
	, fade_( false )
	, cTime_( 0 ) {
	stream_[0] = nullptr;
	stream_[1] = nullptr;
	streamChannel_[0] = nullptr;
	streamChannel_[1] = nullptr;
	playing_[0] = false;
	playing_[1] = false;

	init_();
}

MediaPlayer::~MediaPlayer() {
	free_();
	fmodSystem_->release();
}

bool MediaPlayer::init_() {
	FMOD_RESULT result;

	result = FMOD::System_Create(&fmodSystem_);
	if( result != FMOD_OK ) {
		return false;
	}

	result = fmodSystem_->init(100, FMOD_INIT_NORMAL, 0);
	if( result != FMOD_OK ) {
		return false;
	}

	// inicializace knihovny FMOD
	result = fmodSystem_->setOutput( FMOD_OUTPUTTYPE_DSOUND );	
	if( result != FMOD_OK ) {
		return false;
	}
	result = fmodSystem_->setDriver( 0 ); // TODO: moznost vybrat ovladac na prehravani hudby
	if( result != FMOD_OK ) {
		return false;
	}

	//if( !FSOUND_Init( 44100, 32, 0 ) ) {
	//	FSOUND_Close();
	//	return false;
	//}

//	FSOUND_File_SetCallbacks(MediaPlayer::fmod_open, MediaPlayer::fmod_close, MediaPlayer::fmod_read,
//		MediaPlayer::fmod_seek, MediaPlayer::fmod_tell);

	//FSOUND_Stream_SetBufferSize( 1000 );
	activeStream_ = 1;
	return true;
}

void MediaPlayer::free_() {
	if( stream_[0] ) {
		stream_[0]->release();
	}
	if( stream_[1] ) {
		stream_[1]->release();
	}
	stream_[0] = nullptr;
	stream_[1] = nullptr;
}

void MediaPlayer::openStream( const std::string& fname ) {
	if( streams_ < 2 ) {
		streams_++;
	}
	activeStream_ = 1 - activeStream_;
	if( stream_[activeStream_] ) {
		stream_[activeStream_]->release();
	}
	fmodSystem_->createStream( fname.c_str(), FMOD_DEFAULT, 0, &stream_[activeStream_] );
}

void MediaPlayer::playStream() {
	fmodSystem_->playSound( static_cast<FMOD_CHANNELINDEX>( GameEngine::MUSIC_AUDIO_CHANNEL[activeStream_] ), stream_[activeStream_], false, &streamChannel_[activeStream_] );
	streamChannel_[activeStream_]->setLoopCount(-1);

	playing_[activeStream_] = true;

	if( streams_ > 1 ) {
		fade_ = true;
		cTime_ = 0;
		streamChannel_[activeStream_]->setVolume( 0.0f );
	} else {
		streamChannel_[activeStream_]->setVolume( (float)streamVolume_ );
	}
}

void MediaPlayer::stopStream() {
	streamChannel_[activeStream_]->stop();
	playing_[activeStream_] = false;
}

void MediaPlayer::pauseStream( bool pause ) {
	//FSOUND_SetPaused(streamChannel[activeStream], pause);
	if( pause ) {
		fade_ = M_FADE_OUT;
	} else {
		fade_ = M_FADE_IN;
		streamChannel_[activeStream_]->setVolume( 0 );
		streamChannel_[activeStream_]->setPaused( false );
	}
	cTime_ = 0;
}

void MediaPlayer::setStreamVolume( float volume ) {
	streamVolume_ = volume;
	for( int i = 0; i < 2; i++ )
		if( playing_[i] ) {
			streamChannel_[activeStream_]->setVolume( streamVolume_ );
		}
}

SoundChannel MediaPlayer::playSound( const Sound* sound, float volume ) {
	FMOD::Channel* channel;
	fmodSystem_->playSound( FMOD_CHANNEL_FREE, sound->sample_, false, &channel );
	channel->setVolume(volume);
	return SoundChannel(channel);
}

SoundChannel MediaPlayer::playSound( const std::string& fileName, float volume ) {
	PlayingSound snd;
	snd.sound = SoundManager::i()->createObject( fileName );

	fmodSystem_->playSound( FMOD_CHANNEL_FREE, snd.sound->sample_, false, &snd.channel.channel_ );
	snd.channel.channel_->setVolume(volume);

	sounds_.push_back(snd);
	return snd.channel;
}

SoundChannel MediaPlayer::playSound( int channelIndex, const Sound* sound, float volume ) {
	FMOD::Channel* channel;
	fmodSystem_->playSound( static_cast<FMOD_CHANNELINDEX>( channelIndex ), sound->sample_, false, &channel );
	channel->setVolume(volume);
	return SoundChannel(channel);
}

void MediaPlayer::stopChannel( SoundChannel& channel ) {
	channel.channel_->stop();
}

bool MediaPlayer::channelPlaying( const SoundChannel& channel ) {
	bool res;
	channel.channel_->isPlaying( &res );
	return res;
}

void MediaPlayer::update( unsigned long time ) {
	for( auto it = sounds_.begin(); it != sounds_.end(); ) {
		if( !channelPlaying(it->channel) ) {
			auto tmp = it++;
			sounds_.erase(tmp);
		}
	}

	if( fade_ ) {
		// aktualizace fadingu
		cTime_ += time;
		int fTime = static_cast<int>( fade_ == M_CROSS_FADE ? M_CROSSFADE_TIME : M_FADE_TIME );
		float fadeVolume = ( streamVolume_ * ( static_cast<float>( cTime_ ) / fTime ) );

		if( fade_ == M_FADE_OUT ) {
			fadeVolume = streamVolume_ - fadeVolume;
		}

		if( fadeVolume > streamVolume_ ) {
			fadeVolume = streamVolume_;
			fade_ = 0;
			if( fade_ == M_CROSS_FADE ) {
				streamChannel_[1 - activeStream_]->stop();
			} else if( fade_ == M_FADE_OUT ) {
				streamChannel_[activeStream_]->setPaused( true );
			}
		}

		streamChannel_[activeStream_]->setVolume( fadeVolume );
		if( fade_ == M_CROSS_FADE ) {
			streamChannel_[1 - activeStream_]->setVolume( streamVolume_ - fadeVolume );
		}
	}
}

/*
void* F_CALLBACKAPI MediaPlayer::fmod_open( const char* name ) {
	size_t size;

	CZReader reader;

	// otevre package s hudbou a najde pozadovany soubor
	CFilePackage fp( "music.dat" );
	std::string tmp( name );
	tmp.substr( 4, tmp.length() );
	fp.seekToFile( tmp.c_str() );
	fp.getFileOrigSize( tmp.c_str(), size );

	// cely soubor nacte do bufferu
	SBuffer* b = new SBuffer( size );
	reader.read( fp.getStream(), b->buffer, size );
	return b;
}

void F_CALLBACKAPI MediaPlayer::fmod_close( void* handle ) {
	// smaze buffer
	SBuffer* buffer = ( SBuffer* )handle;
	delete buffer;
}

int F_CALLBACKAPI MediaPlayer::fmod_tell( void* handle ) {
	// vrati pozici v bufferu
	SBuffer* buffer = ( SBuffer* )handle;
	return 	buffer->pos;
}

int F_CALLBACKAPI MediaPlayer::fmod_seek( void* handle, int pos, signed char mode ) {
	// seek v bufferu
	SBuffer* buffer = ( SBuffer* )handle;
	buffer->seek( pos, mode );
	return 1;
}

int F_CALLBACKAPI MediaPlayer::fmod_read( void* buffer, int size, void* handle ) {
	// cteni z bufferu
	SBuffer* b = ( SBuffer* )handle;
	int s = b->read( ( char* )buffer, size );
	return s;
}
*/

MediaPlayer* MediaPlayer::i() {
	static MediaPlayer instance;
	return &instance;
}

void MediaPlayer::loadSound( Sound* sound, const std::string& fname ) {
	fmodSystem_->createSound( fname.c_str(), FMOD_DEFAULT, 0, &sound->sample_ );
}

void MediaPlayer::setChannelVolume( SoundChannel& channel, float volume ) {
	channel.channel_->setVolume(volume);
}

}

// LAE
#include "GameManager.h"
#include "../graphics/Screen.h"
#include "../cache/ImageCache.h"
#include "../cache/AnimationCache.h"
#include "../cache/SoundCache.h"
#include "../cache/TextStorage.h"
#include "../audio/MediaPlayer.h"
#include "../game/Global.h"
#include "../game/GameTimer.h"
// log
#include "../logger/Logger.h"
#include "../logger/OStreamContext.h"
// stl
#include <iostream>
#include <sstream>
// win32
#ifdef WIN32
#	include <windows.h>
#	include <psapi.h>
#endif

namespace LAE {

GameManager::GameManager() 
	: screen_( Global::i()->screenWidth(), Global::i()->screenHeight(), Global::i()->screenDepth(), true )
	, startTime_( 0 )
	, timeElapsed_( 0 ) {
	GameTimer::i()->setDiscrete(true);
}

GameManager::~GameManager() {
}

void GameManager::addScreen( GameScreen* screen ) {
	screens_.push_back( screen );
}

void GameManager::outputDebug( Screen& scr ) {
	std::stringstream str;
#ifdef WIN32
	str << "RAM: ";

	HANDLE process = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;

	if( GetProcessMemoryInfo( process, &pmc, sizeof( pmc ) ) ) {
		str << ( pmc.WorkingSetSize / 1024 ) << " / " << ( pmc.PeakWorkingSetSize / 1024 ) << " KB; ";
	}
#endif

	str << "TimeElapsed: " << timeElapsed_;
	scr.drawText( str.str(), *Global::i()->defaultFont(), 0, 0, Color(255, 0, 0) );
}

void GameManager::init() {
	// screen
	Screen screen( Global::i()->screenWidth(), Global::i()->screenHeight(), 32, false );

	// global
	if( !Global::i()->init() ) {
		return ;
	}

	// volume
	LOG_DEBUG( "Setting stream volume to " << ( int )Global::i()->getMusicVolume() );
	MediaPlayer::i()->setStreamVolume( Global::i()->getMusicVolume() );

	startTime_ = 0;
	timeElapsed_ = 0;
	for( GameScreens::iterator i = screens_.begin(); i != screens_.end(); ++i ) {
		( *i )->init();
	}
}

bool GameManager::update( const SMouseState& mouse, const SKeyboardState& keyboard ) {
	GameTimer::i()->updateTime();
	if( startTime_ != 0 ) {
		timeElapsed_ = std::min<unsigned long>( GameTimer::i()->getTime() - startTime_, 1000 );
	} else {
		timeElapsed_ = 0;
	}
	startTime_ = GameTimer::i()->getTime();

	GameScreen::EScreenState state;
	for( GameScreens::iterator i = screens_.begin(); i != screens_.end(); ++i ) {
		state = ( *i )->update( mouse, keyboard, timeElapsed_ );
		( *i )->render( screen_ );

		if( state == GameScreen::ENDED ) {
			return false;
		}
	}
	
	MediaPlayer::i()->update( timeElapsed_ );
	Global::i()->update( timeElapsed_ );

#ifdef _DEBUG
	outputDebug( screen_ );
#endif

	screen_.flip();
	return true;
}

void GameManager::free() {
	AnimationManager::i()->clearAll();
	ImageManager::i()->clearAll();
	SoundManager::i()->clearAll();
}

}

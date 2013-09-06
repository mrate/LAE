#include "GameTimer.h"

#ifdef WIN32
#	include <windows.h>
#else
#	include <sys/time.h>
#	include <time.h>
#	include <stdint.h>
#endif

// stl
#include <cassert>

namespace LAE {

GameTimer::GameTimer()
	: isDiscrete_( false )
	, currentTime_( getSystemTime() ) {
}

GameTimer::~GameTimer() {
}

GameTimer* GameTimer::i() {
	static GameTimer instance;
	return &instance;
}

unsigned long GameTimer::getTime() const {
	if( isDiscrete_ ) {
		return currentTime_;
	} else {
		return getSystemTime();
	}
}

void GameTimer::setDiscrete( bool discrete ) {
	isDiscrete_ = discrete;
	if( isDiscrete_ ) {
		currentTime_ = getSystemTime();
	}
}

bool GameTimer::isDiscrete() const {
	return isDiscrete_;
}

void GameTimer::updateTime() {
	assert( isDiscrete_ );
	if( isDiscrete_ ) {
		currentTime_ = getSystemTime();
	}
}

void GameTimer::updateTime( unsigned long diff ) {
	assert( isDiscrete_ );
	if( isDiscrete_ ) {
		currentTime_ += diff;
	}
}

unsigned long GameTimer::getSystemTime() const {
#ifdef WIN32
	return GetTickCount();
#else
	struct timespec t;
	clock_gettime( CLOCK_MONOTONIC, &t );
	uint64_t p = ( ( ( uint64_t )t.tv_sec ) * 1000 ) + ( ( ( uint64_t )t.tv_nsec ) / 1000000 );
	return ( unsigned long )p;
#endif
}

}

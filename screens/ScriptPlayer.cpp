// LAE
#include "ScriptPlayer.h"
#include "../logger/Logger.h"
#include "../game/Global.h"
#include "../types/types.h"
#include "../cache/ImageCache.h"
#include "../exception/Exception.h"

namespace LAE {

ScriptPlayer::ScriptPlayer() 
	: scriptContext_( nullptr )
	, timeToExit_( false )
	, roomPaused_( false )
	, scriptPaused_( false ) {
}

ScriptPlayer::~ScriptPlayer() {
	free();
}

void ScriptPlayer::init() {
	LOG_DEBUG( "ScriptPlayer::init" );

	scriptContext_ = interpret_.createNewContext();
}

void ScriptPlayer::loadScript( const std::string& fname ) {
	loadScript(fname, "init");
}

void ScriptPlayer::loadScript( const std::string& fname, const std::string& funcName ) {
	free();
	init();

	interpret_.loadFile( scriptContext_, fname );
	interpret_.registerObject( scriptContext_, "scriptPlayer", this );
	// call init func
	interpret_.runFunction( scriptContext_, funcName );
}

void ScriptPlayer::render( Screen& scr ) {
	if( roomPlayer_.get() && !roomPaused_ ) {
		roomPlayer_->render( scr );
	} else {
		interpret_.runFunction( scriptContext_, "render", scr );
		Global::i()->getCursor().render( scr, 0, 0 );
	}
}

GameScreen::EScreenState ScriptPlayer::update( const SMouseState& mouse, const SKeyboardState& keyboard, unsigned long timeMili ) {
	if( !pendingScriptName_.empty() ) {
		loadScript( pendingScriptName_, pendingScriptFunction_ );
		pendingScriptName_.clear();
		pendingScriptFunction_.clear();
	} else {
		if( roomPlayer_.get() && !roomPaused_ ) {
			GameScreen::EScreenState state = roomPlayer_->update( mouse, keyboard, timeMili );

			switch( state ) {
			case GameScreen::PAUSED:
				roomPaused_ = true;
				interpret_.resume(scriptContext_, 0);
				scriptPaused_ = false;
				break;

			case GameScreen::ENDED:
				roomPlayer_.reset();
				interpret_.resume(scriptContext_, 1);
				scriptPaused_ = false;
				break;
			default:
				break;
			}
		} else {
			interpret_.runFunction( scriptContext_, "update", keyboard, mouse, timeMili );
		}
	}

	return timeToExit_ ? GameScreen::ENDED : GameScreen::RUNNING;
}

AnimatedSprite* ScriptPlayer::createSprite() {
	sprites_.push_back( new AnimatedSprite() );
	return sprites_.back();
}

void ScriptPlayer::loadRoom( const std::string& fname ) {
	roomPlayer_ = TSHPRoomPlayer( new RoomPlayer() );
	roomPlayer_->init();
	roomPlayer_->loadRoom( fname );
	scriptPaused_ = true;
}

void ScriptPlayer::resumeRoom() {
	if( roomPlayer_.get() && roomPaused_ ) {
		roomPaused_ = false;
		scriptPaused_ = true;
	} else {
		interpret_.resume( scriptContext_, 1 );
	}
}

void ScriptPlayer::exit() {
	timeToExit_ = true;
}

void ScriptPlayer::prepareScript( const std::string& scriptName, const std::string& funcName ) {
	pendingScriptName_ = scriptName;
	pendingScriptFunction_ = funcName;
}

void ScriptPlayer::free() {
	if( scriptContext_ ) {
		delete scriptContext_;
		scriptContext_ = nullptr;
	}

	std::for_each( sprites_.begin(), sprites_.end(), Deleter<AnimatedSprite>() );
	std::for_each( images_.begin(), images_.end(), Deleter<Image>() );
}

}

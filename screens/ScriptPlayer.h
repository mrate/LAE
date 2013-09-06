#pragma once
#ifndef _SCRIPTPLAYER_H_
#define _SCRIPTPLAYER_H_

// LAE
#include "GameScreen.h"
#include "RoomPlayer.h"
#include "../graphics/AnimatedSprite.h"
#include "../script/ScriptInterpret.h"
// stl
#include <vector>

namespace LAE {

class ScriptPlayer;
typedef boost::shared_ptr<ScriptPlayer>		TSHPScriptPlayer;

class ScriptPlayer : public GameScreen {
public:
	ScriptPlayer();
	virtual ~ScriptPlayer();

	void init();
	void loadScript( const std::string& fname );
	void loadScript( const std::string& fname, const std::string& funcName );
	void prepareScript( const std::string& scriptName, const std::string& funcName );
	void render( Screen& scr );
	GameScreen::EScreenState update( const SMouseState& mouse, const SKeyboardState& keyboard, unsigned long timeMili );

	// lua
	AnimatedSprite* createSprite();
	void			loadRoom( const std::string& fname );
	void			resumeRoom();
	void			exit();

private:
	AnimatedSprites	sprites_;	///< sprity
	TImageVector	images_;		///< obrazky

	TSHPScriptPlayer	scriptPlayer_;	///< vnoreny prehravac skriptu
	TSHPRoomPlayer		roomPlayer_;		///< vnoreny prehravac mistnosti

	ScriptInterpret	interpret_;
	ScriptContext* scriptContext_;

	bool	timeToExit_;	///< ukonceni cinnosti
	bool	roomPaused_;	///< mistnost je pauznuta

	bool	scriptPaused_;

	std::string pendingScriptName_;
	std::string pendingScriptFunction_;

	void free();
};

}

#endif // _SCRIPTPLAYER_H_

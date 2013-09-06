#pragma once
#ifndef _ROOMPLAYER_H_
#define _ROOMPLAYER_H_

// LAE
#include "Events.h"
#include "GameRoom.h"
#include "GameScreen.h"
#include "../types/types.h"
#include "../graphics/AnimatedSprite.h"
#include "../graphics/TextPanel.h"
#include "../objects/Camera.h"
#include "../objects/InventoryItem.h"
#include "../objects/Item.h"
#include "../audio/Sound.h"
#include "../audio/SoundChannel.h"
#include "../cache/TextStorage.h"
#include "../script/ScriptInterpret.h"
// boost
#include <boost/shared_ptr.hpp>
// stl
#include <string>
#include <vector>
#include <map>
#include <deque>

namespace LAE {
class RoomPlayer;

typedef boost::shared_ptr<RoomPlayer>	TSHPRoomPlayer;

class RoomPlayer : public GameScreen {
public:
	RoomPlayer();
	virtual ~RoomPlayer();

	void init();
	void free();

	void render( Screen& scr );
	GameScreen::EScreenState update( const SMouseState& mouse, const SKeyboardState& keyboard, unsigned long timeMili );

	void loadRoom( const std::string& fname );
	bool waitLoadRoom( SBaseWaitEvent* event );

	//////////////////////////////////////////////////////////////////////////
	void waitActor();
	void goActor( int x, int y );
	void wait( unsigned long time );
	void showSpeechText( const std::string& text, short x, short y );
	void showSpeechText( const std::string& text, double x, double y );
	void hideSpeechText();

	void waitCamera();
	void activateBox( unsigned char boxIndex, bool active );

	void cacheAnim( const std::string& name );
	void uncacheAnim( const std::string& name );

	void cacheSound( const std::string& name );
	void uncacheSound( const std::string& name );

	int dialogChoice( const std::string* choices, unsigned char choicesCount );
	void say( Actor* actor, const char* text );
	void say( Actor* actor, const std::string& text, const std::string& snd );
	void waitAnimation( const AnimationPlayer* animation );
	void inventoryInsert( const std::string& name );
	void inventoryRemove( const std::string& name );
	void scrLoadRoom( const std::string& fname );

private:
	enum { INVALID_INVENTORY_ITEM = -1 };

	/// cas zatmeni
	enum { FADE_TIMEOUT = 2500 };

	/// typ zatmeni
	enum EFadeType {
	    FADE_NONE,	///< zadne stmivani
	    FADE_IN,	///< rozetmivani
	    FADE_OUT	///< zatmivani
	};

	// strings methods
	void loadStringMap( const std::string& fname );
	const std::string& getText( const std::string& name ) const;

	// script methods
	ScriptContext* createContext();
	void loadScript( ScriptContext* context, const std::string& fname ) const;
	void registerString( ScriptContext* context, const std::string& name, const std::string& str ) const;
	void registerItem( ScriptContext* context , Item& item ) const;
	void setActiveContext(ScriptContext* context);

	void runExamineFunc( ScriptContext* context, const std::string& func );
	void runUseFunc( ScriptContext* context, const std::string& func );

	// path methods
	void findPath( int x, int y, Actor* actor ) const;
	void findPath( int srcBox, int dstBox, int* pred, const Point& start, const Point& end, std::deque<Point>& path ) const;
	void moveCloser( Point& point, const Point& toPoint ) const;
	void findNearestBox( int x, int y, unsigned char srcBox, unsigned char& dstBox, Point& nearest ) const;

	// fade in/out
	void fadeIn();
	void fadeOut();

	void setActiveInventoryItem( short inventoryItem );

	// rendering methods
	void renderInventory( Screen& screen );
	void renderDialog( Screen& screen );
	void renderDebug( Screen& screen );
	void renderFade( Screen& screen );
	void renderCursor( Screen& screen );

	// event methods
	unsigned long getWaitEvent() const;
	void setWaitEvent( SBaseWaitEvent* event );
	void resetWaitEvent();
	void handleEvents( const SMouseState& mouse, const SKeyboardState& keyboard, unsigned long timeMili );

	//////////////////////////////////////////////////////////////////////////
	// >> DEBUG
	Actor debugActor_;
	bool drawDebugPath_;
	mutable int debugNearestBox_;
	mutable TextPanel debugPanel_;

	void renderPath( Screen& screen, Actor* actor ) const;
	void drawNicePoly( Screen& screen, const Point* p, const Color& color ) const;
	// << DEBUG
	//////////////////////////////////////////////////////////////////////////

	short activeInvItem_;	///< aktivni predmet z inventare (-1 = zadny aktivni predmet)
	unsigned long fadeTime_;
	EFadeType fadeType_;

	Camera	camera_;

	// screen
	int cursorX_;
	int cursorY_;
	bool cursorVisible_;

	// description
	struct Description : public Point {
		std::string		text;
		bool			enabled;

		Description() : enabled( false ) {}
	} description_;

	// speech
	struct SpeechText : public Point {
		Sound				sound;
		SoundChannel		soundChannel;
		bool				enabled;
		std::string			text;

		SpeechText() : enabled( false ) {}
	} speechText_;

	/// dialog
	struct Dialog {
		char			selectedAnswer;		///< vybrana odpoved
		StringVector	choices;			///< moznosti

		Dialog() : selectedAnswer( 0 ) {}
	} dialog_;

	//Actor	actors_[1];		///< postavy v mistnosti
	Actor*	player_;

	// room properties
	GameRoom	room_;
	TextStorage texts_;

	// wait to event
	TWaitEvent	waitEvent_;

	// script
	ScriptInterpret interpret_;
	ScriptContext* scriptRoom_;
	ScriptContext* scriptInventory_;
	ScriptContext* scriptActive_;

	bool		scriptRunning_;	///< probiha skript
	bool		scriptWaiting_;	///< skript ceka na udalost
	bool		scriptSkipping_;	///< skript se preskakuje

	bool		debug_;	///< zobrazovat debugovaci informace

	std::vector< PAnimation > animationCache_;
	std::vector< PSound > soundCache_;
};

}

#endif // _ROOMPLAYER_H_

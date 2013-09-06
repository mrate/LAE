#pragma once

#include "../graphics/Screen.h"

namespace LAE {
class GameScreen {
public:
	enum EScreenState {
	    RUNNING,
	    PAUSED,
	    ENDED
	};

	GameScreen() {};
	virtual ~GameScreen() {};

	virtual void init() = 0;
	virtual void render( Screen& scr ) = 0;
	virtual EScreenState update( const SMouseState& mouse, const SKeyboardState& keyboard, unsigned long timeMili ) = 0;
};

typedef std::vector<GameScreen*> GameScreens;
}

#pragma once

#include "../graphics/Screen.h"
#include "GameScreen.h"
#include <vector>

namespace LAE {

class GameManager {
public:
	GameManager();
	virtual ~GameManager();

	void addScreen( GameScreen* screen );

	void init();
	bool update( const SMouseState& mouse, const SKeyboardState& keyboard );
	void free();

private:
	Screen screen_;
	GameScreens screens_;

	unsigned long startTime_;
	unsigned long timeElapsed_;

	void outputDebug( Screen& scr );
};

}

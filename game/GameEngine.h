#pragma once
#ifndef _GAMEENGINE_H_
#define _GAMEENGINE_H_

#include "../objects/Actor.h"

namespace LAE {

class GameEngine {
public:
	static const int MUSIC_AUDIO_CHANNEL[];
	static const int SPEECH_AUDIO_CHANNEL;

	Actor* getPlayer() { return &player_; }

	static GameEngine* i();

private:
	GameEngine() {}
	~GameEngine() {}

	Actor player_;
};

}

#endif // _GAMEENGINE_H_

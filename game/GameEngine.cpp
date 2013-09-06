#include "GameEngine.h"

namespace LAE {

const int GameEngine::MUSIC_AUDIO_CHANNEL[] = { 0, 1 };

GameEngine* GameEngine::i() {
	static GameEngine instance;
	return &instance;
}

const int GameEngine::SPEECH_AUDIO_CHANNEL = 2;

}

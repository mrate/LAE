#include "SoundCache.h"

namespace LAE {

template<>
SoundManager* SoundManager::instance = 0;

namespace {
	SoundManager::Initializer i("Sound", "sfx\\");
}

}


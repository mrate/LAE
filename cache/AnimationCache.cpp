#include "AnimationCache.h"

namespace LAE {

template<>
AnimationManager* AnimationManager::instance = nullptr;

namespace {
	AnimationManager::Initializer i("Animation", "anims\\");
}

}


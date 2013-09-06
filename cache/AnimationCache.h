#pragma once
#ifndef _ANIMATIONCACHE_H_
#define _ANIMATIONCACHE_H_

#include "CacheManager.h"
#include "../graphics/Animation.h"

namespace LAE {

typedef CacheManager<Animation> AnimationManager;

}

#endif // _ANIMATIONCACHE_H_

//#pragma once
//
//#include "Cache.h"
//#include "../graphics/Animation.h"
//
//namespace LAE {
//typedef Cache<Animation> AnimationCache;
//
//template<>
//AnimationCache::HMap_t AnimationCache::cacheMap;
//
//template<>
//std::string AnimationCache::prefix = "Animation";
//
//template<>
//std::string AnimationCache::pathPrefix = "anims\\";
//}

#pragma once
#ifndef _SOUNDCACHE_H_
#define _SOUNDCACHE_H_

#include "CacheManager.h"
#include "../audio/Sound.h"

namespace LAE {

	typedef CacheManager<Sound> SoundManager;

}

#endif // _SOUNDCACHE_H_

//#pragma once
//
//#include "../audio/Sound.h"
//#include "Cache.h"
//
//namespace LAE {
//typedef Cache<Sound> SoundCache;
//
//template<>
//SoundCache::HMap_t SoundCache::cacheMap;
//
//template<>
//std::string SoundCache::prefix = "Sound";
//
//template<>
//std::string SoundCache::pathPrefix = "sfx\\";
//}

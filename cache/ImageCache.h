#pragma once
#ifndef _IMAGECACHE_H_
#define _IMAGECACHE_H_

#include "CacheManager.h"
#include "../graphics/Image.h"

namespace LAE {
typedef CacheManager<Image> ImageManager;
}

#endif // _IMAGECACHE_H_

//#pragma once
//
//#include "Cache.h"
//#include "../graphics/Image.h"
//
//namespace LAE {
//typedef Cache<Image> ImageCache;
//
//template<>
//ImageCache::HMap_t ImageCache::cacheMap;
//
//template<>
//std::string ImageCache::prefix = "Image";
//
//template<>
//std::string ImageCache::pathPrefix = "gfx\\";
//}

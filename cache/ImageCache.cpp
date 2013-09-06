#include "ImageCache.h"

namespace LAE {

template<>
ImageManager* ImageManager::instance = 0;

namespace {
	ImageManager::Initializer i("Image", "gfx\\");
}

}


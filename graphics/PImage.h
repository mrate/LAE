#pragma once
#ifndef _PIMAGE_H_
#define _PIMAGE_H_

#include <boost/shared_ptr.hpp>

namespace LAE {

class Image;
typedef boost::shared_ptr<const Image> PImage;

}

#endif // _PIMAGE_H_

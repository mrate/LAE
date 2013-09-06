#pragma once

// stl
#include <string>
#include <vector>
// boost
#include <boost/shared_ptr.hpp>
// png
#include <png.h>
// LAE
#include "../cache/CacheableObject.h"
#include "opengl.h"

namespace LAE {
class Image {
public:
	Image();
	virtual ~Image();

	void load( const std::string& name );
	void free();

	const std::string& getFileName() const { return fileName_; }

	int width() const { return imgWidth_; }
	int height() const { return imgHeight_; }

private:
	GLuint texture_;
	int imgWidth_;
	int imgHeight_;
	bool loaded_;
	std::string fileName_;

	static void Readfunc( png_structp pngPtr, png_bytep data, png_size_t length );

	friend class Screen;
};

typedef std::vector<Image*>	TImageVector;

typedef boost::shared_ptr<const Image> PImage;

}

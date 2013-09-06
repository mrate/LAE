	#pragma once

// LAE
#include "PImage.h"
// stl
#include <string>
#include <map>

namespace LAE {

/// Trida bitmap fontu
class Font {
public:
	Font();
	virtual ~Font();

	void loadBin( const std::string& fname );
	void load( const std::string& fname, const std::string& textFile );

	int getWidth( const std::string& text ) const;

	void setImage( PImage image ) { image_ = image; }
	PImage getImage() const { return image_; }

	unsigned char getCharWidth() const { return charWidth_; }
	unsigned char getCharHeight() const { return charHeight_; }

private:
	friend class Screen;

	PImage image_;
	unsigned char left_[256];
	unsigned char right_[256];
	unsigned char charMap_[256];
	unsigned char charWidth_;
	unsigned char charHeight_;
	unsigned char charCols_;
	unsigned char charRows_;
};

typedef std::map<std::string, Font> FontMap;
}

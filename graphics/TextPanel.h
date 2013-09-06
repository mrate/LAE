#ifndef TEXTPANEL_H_
#define TEXTPANEL_H_

// LAE
#include "Color.h"
// stl
#include <sstream>

namespace LAE {

class Screen;
class Font;

class TextPanel {
public:
	TextPanel();
	~TextPanel();

	void setX( int x ) { x_ = x; }
	int getX() const { return x_; }
	void setY( int y ) { y_ = y; }
	int getY() const { return y_; }

	void setWidth( int width ) { width_ = width; }
	int getWidth() const { return width_; }
	void setHeight( int height ) { height_ = height; }
	int getHeight() const { return height_; }

	void autoResize();

	void update( unsigned long millis );
	void render( Screen& screen ) const;

	template<typename T>
	TextPanel& operator<<( T t ) {
		stream_ << t;
		return *this;
	}

	void reset();

	void setBgColor( const Color& color ) { bgColor_ = color; }
	void setTextColor( const Color& color ) { fontColor_ = color; }

private:
	std::stringstream stream_;

	const Font* font_;

	int x_;
	int y_;
	int width_;
	int height_;

	Color bgColor_;
	Color fontColor_;
};

}

#endif /* TEXTPANEL_H_ */
